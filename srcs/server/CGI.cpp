//
// Created by Kester kas De rooij on 3/31/22.
//
#include "server.hpp"
#include <unistd.h>
#include <sys/stat.h>
#include "../utils/webserv.hpp"

static HTTPResponseMessage::e_responseStatusCode createPath( webserv::locationData location, webserv::Request request, char **args ){
    std::string reqPath = location.root;
    struct stat buffer;

    reqPath.append( location.cgi_param );
    args[0] = strdup( "/usr/bin/python" );
    args[1] = strdup( reqPath.c_str());

    if ( stat( args[1], &buffer ) != 0 )
        return HTTPResponseMessage::NOT_FOUND;
    if ( request.getMethod() == webserv::Request::POST ) {
        args[2] = strdup( request.getBody().c_str());
        if ( !args[2] )
            return HTTPResponseMessage::BAD_REQUEST;
    }
    return HTTPResponseMessage::OK;
}

static HTTPResponseMessage::e_responseStatusCode executeCmd( int* pipes, char** args, char** env, int* pid_storage ){
    int ret, pid = fork();
    if ( pid < 0 ) {
        return HTTPResponseMessage::INTERNAL_SERVER_ERROR;
    } else if ( pid == 0 ) {
        close( pipes[0] );
        dup2( pipes[1], STDERR_FILENO );
        dup2( pipes[1], STDOUT_FILENO );
        close( pipes[1] );
        if (( ret = execve( args[0], args, env )) != 0 )
            std::cout << "file open went wrong" << std::endl;
        exit( ret );
    }
    close( pipes[1] );
    *pid_storage = pid;
    free( args[0] );
    if ( args[1] )
        free( args[1] );
    if ( args[2] )
        free( args[2] );
    return HTTPResponseMessage::OK;
}

HTTPResponseMessage::e_responseStatusCode
CGI_register( webserv::locationData location, webserv::serverData &serverData, int client_fd,
              webserv::Request request ) {
    HTTPResponseMessage::e_responseStatusCode ret;
    int pipes[2];
    std::string ret_str;
    char *args[4] = { NULL, NULL, NULL, NULL };

    ret = createPath( location, request, args );
    if ( ret != HTTPResponseMessage::OK )
        return ret;

    if ( pipe( pipes ) != 0 )
        return HTTPResponseMessage::INTERNAL_SERVER_ERROR;
    serverData.cgi_responses[pipes[0]].client_fd = client_fd;

    struct kevent new_socket_change;
    EV_SET( &new_socket_change, pipes[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL );
    if ( kevent( serverData.kqData.kq, &new_socket_change, 1, NULL, 0, NULL ) == ERROR ){
        webserv::kqueueFailure( pipes[0] );
        return HTTPResponseMessage::INTERNAL_SERVER_ERROR;
    }

    ret = executeCmd( pipes, args, serverData.clientSockets[client_fd]->env, &serverData.cgi_responses[pipes[0]].pid);
    if ( ret != HTTPResponseMessage::OK )
        return ret;
    return HTTPResponseMessage::OK;
}

HTTPResponseMessage CGI_attempt( int pipe_fd, webserv::cgi_response resp, webserv::httpData config ) {
    HTTPResponseMessage response;
    int ret = SUCCES, status;
    std::string ret_str;
    struct stat sb{ };

    waitpid( resp.pid, &status, 0 );
    fstat( pipe_fd, &sb );
    ret_str.resize( sb.st_size );
    read( pipe_fd, ( char * ) ( ret_str.data()), sb.st_size );
    close( pipe_fd );
    if ( WIFEXITED( status ))
        ret = WEXITSTATUS( status );
    std::cerr << "ret " << ret << std::endl;
    if ( ret == 0 ) {
        response.addStatus( HTTPResponseMessage::OK )
                .addLength( ret_str.length())
                .addBody( ret_str )
                .addType( "text/plain" );
    } else {
        response.addStatus( HTTPResponseMessage::INTERNAL_SERVER_ERROR )
                .addLength( config.error_page[HTTPResponseMessage::INTERNAL_SERVER_ERROR].length())
                .addBody( config.error_page[HTTPResponseMessage::INTERNAL_SERVER_ERROR] )
                .addType( "text/html" );
    }
    return response;
}

int responseFromCGI( webserv::serverData &serverData, int pipe_fd ) {
    HTTPResponseMessage response;
    std::string line;
    std::string body( "" );
    webserv::cgi_response resp = serverData.cgi_responses[pipe_fd];

    response = CGI_attempt( pipe_fd, resp, *serverData.clientSockets[resp.client_fd] );
    serverData.requests.erase( resp.client_fd );
    printf( "  register CGI respond event - %d\n", resp.client_fd );
    struct kevent new_socket_change;
    EV_SET( &new_socket_change, resp.client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL );
    if ( kevent( serverData.kqData.kq, &new_socket_change, 1, NULL, 0, NULL ) == ERROR ){
        webserv::kqueueFailure( resp.client_fd );
        return ERROR;
    }
    serverData.responses[resp.client_fd] = response.toString();
    return SUCCES;
}
 