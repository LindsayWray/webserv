#include "CGI.hpp"
#include "../webserver/webserv.hpp"

using namespace webserv;

static HTTPResponseMessage::e_responseStatusCode createPath( locationData location, Request request, char** args ) {
    std::string reqPath = location.root;
    struct stat buffer;

    reqPath.append( location.cgi_param );
    args[0] = strdup( "/usr/bin/python" );
    args[1] = strdup( reqPath.c_str() );

    if ( stat( args[1], & buffer ) != 0 )
        return HTTPResponseMessage::NOT_FOUND;
    if ( request.getMethod() == Request::POST ) {
        args[2] = strdup( request.getBody().c_str() );
        if ( !args[2] )
            return HTTPResponseMessage::BAD_REQUEST;
    }
    return HTTPResponseMessage::OK;
}

static HTTPResponseMessage::e_responseStatusCode executeCmd( int* pipes, char** args, char** env, int* pid_storage ) {
    int ret, pid = fork();
    if ( pid < 0 ) {
        return HTTPResponseMessage::INTERNAL_SERVER_ERROR;
    } else if ( pid == 0 ) {
        close( pipes[0] );
        dup2( pipes[1], STDERR_FILENO );
        dup2( pipes[1], STDOUT_FILENO );
        close( pipes[1] );
        if ( ( ret = execve( args[0], args, env ) ) != 0 )
            std::cout << "file open went wrong" << std::endl;
        exit( ret );
    }
    close( pipes[1] );
    * pid_storage = pid;
    free( args[0] );
    if ( args[1] )
        free( args[1] );
    if ( args[2] )
        free( args[2] );
    return HTTPResponseMessage::OK;
}

HTTPResponseMessage::e_responseStatusCode webserv::CGIRegister( locationData location, serverData& serverData, int client_fd, Request request ) {
    HTTPResponseMessage::e_responseStatusCode ret;
    int pipes[2];
    std::string ret_str;
    char* args[4] = { NULL, NULL, NULL, NULL };

    ret = createPath( location, request, args );
    if ( ret != HTTPResponseMessage::OK )
        return ret;

    if ( pipe( pipes ) != 0 )
        return HTTPResponseMessage::INTERNAL_SERVER_ERROR;
    serverData.cgiResponses[pipes[0]].clientFd = client_fd;

    struct kevent new_socket;
    EV_SET( & new_socket, pipes[0], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL );
    if ( kevent( serverData.kqData.kq, & new_socket, 1, NULL, 0, NULL ) == ERROR ){
		webserv::kqueueFailure( pipes[0] );
		serverData.cgiResponses.erase(pipes[0]);
		return HTTPResponseMessage::INTERNAL_SERVER_ERROR;
	}

    ret = executeCmd( pipes, args, serverData.clientSockets[client_fd].env, & serverData.cgiResponses[pipes[0]].pid );
    if ( ret != HTTPResponseMessage::OK )
        return ret;
    return HTTPResponseMessage::OK;
}
