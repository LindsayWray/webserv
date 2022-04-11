//
// Created by Kester kas De rooij on 3/31/22.
//
#include "server.hpp"
#include <unistd.h>
#include <sys/stat.h>

HTTPResponseMessage::e_responseStatusCode CGI_register( webserv::locationData location, webserv::serverData &serverData, char **env, int client_fd, webserv::Request request ) {
    int pipes[2], ret;
    std::string ret_str, reqPath = location.root;
    char *args[4] = { NULL, NULL, NULL, NULL };

    reqPath.append( location.cgi_param );
    args[0] = strdup( "/usr/bin/python" );
    args[1] = strdup( reqPath.c_str());
    if ( request.getMethod() == webserv::Request::POST )
        args[2] = strdup( request.getBody().c_str() );

    if ( pipe( pipes ) != 0 )
        return HTTPResponseMessage::INTERNAL_SERVER_ERROR;
	serverData.cgi_responses[pipes[0]].client_fd = client_fd;

    struct kevent new_socket_change;
    EV_SET( &new_socket_change, pipes[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL );
    if ( kevent( serverData.kqData.kq, &new_socket_change, 1, NULL, 0, NULL ) == ERROR )
        return HTTPResponseMessage::INTERNAL_SERVER_ERROR;

    int pid = fork();
    if ( pid < 0 ) {
        return HTTPResponseMessage::INTERNAL_SERVER_ERROR;
    } else if ( pid == 0 ) {
        close( pipes[0] );
        dup2( pipes[1], STDOUT_FILENO );
        close( pipes[1] );
        if (( ret = execve( args[0], args, env )) < 0 )
            std::cerr << "file open went wrong" << std::endl;
        exit( ret );
    }
	close(pipes[1]);
	serverData.cgi_responses[pipes[0]].pid = pid;
	free( args[0] );
    free( args[1] );
    if ( ret != 0 )
        return HTTPResponseMessage::INTERNAL_SERVER_ERROR;
    return HTTPResponseMessage::OK;
}

std::string CGI_attempt( int pipe_fd, webserv::cgi_response resp ) {
    HTTPResponseMessage response;
    int ret = EXIT_FAILURE, status;
    std::string ret_str;
    struct stat sb{ };

    waitpid( resp.pid, &status, 0 );
    fstat( pipe_fd, &sb );
    ret_str.resize( sb.st_size );
    read( pipe_fd, ( char * ) ( ret_str.data()), sb.st_size );
    close( pipe_fd );
    if ( WIFEXITED( status ))
        ret = WIFEXITED( status );
    return ret_str;
}

int responseFromCGI( webserv::serverData &serverData, int pipe_fd ) {
    HTTPResponseMessage response;
    std::string line;
    std::string body( "" );
	webserv::cgi_response resp = serverData.cgi_responses[pipe_fd];

    body = CGI_attempt( pipe_fd, resp );
    response.addStatus( HTTPResponseMessage::OK )
            .addLength( body.length())
            .addBody( body )
            .addType( "text/plain" );
    serverData.responses[resp.client_fd] = response.toString();
    serverData.requests.erase( resp.client_fd );
    printf( "  register CGI respond event - %d\n", resp.client_fd );
    struct kevent new_socket_change;
    EV_SET( &new_socket_change, resp.client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL );
    int ret = kevent( serverData.kqData.kq, &new_socket_change, 1, NULL, 0, NULL );
    if ( ret == ERROR )
        return -1;
    return 0;
}
 