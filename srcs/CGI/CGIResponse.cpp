#include "CGI.hpp"

using namespace webserv;

static HTTPResponseMessage CGIAttempt( int pipe_fd, cgi_response resp, httpData config ) {
    HTTPResponseMessage response;
    int ret = SUCCES, status;
    std::string ret_str;
    struct stat sb{ };

    waitpid( resp.pid, & status, 0 );
    fstat( pipe_fd, & sb );
    ret_str.resize( sb.st_size );
    read( pipe_fd, ( char* ) ( ret_str.data() ), sb.st_size );
    close( pipe_fd );
    if ( WIFEXITED( status ) )
        ret = WEXITSTATUS( status );
    if ( ret == 0 ) {
        response.addStatus( HTTPResponseMessage::OK )
                .addLength( ret_str.length() )
                .addBody( ret_str )
                .addType( "text/plain" );
    } else {
        response.addStatus( HTTPResponseMessage::INTERNAL_SERVER_ERROR )
                .addLength( config.errorPage[HTTPResponseMessage::INTERNAL_SERVER_ERROR].length() )
                .addBody( config.errorPage[HTTPResponseMessage::INTERNAL_SERVER_ERROR] )
                .addType( "text/html" );
    }
    return response;
}

void webserv::responseFromCGI( serverData& serverData, int pipe_fd ) {
    HTTPResponseMessage response;
    std::string line;
    std::string body( "" );
    cgi_response resp = serverData.cgiResponses[pipe_fd];

    response = CGIAttempt( pipe_fd, resp, serverData.clientSockets[resp.clientFd] );
    serverData.responses[resp.clientFd] = response.toString();
    serverData.requests.erase( resp.clientFd );
    printf( "  register CGI respond event - %d\n", resp.clientFd );
    struct kevent new_socket_change;
    EV_SET( & new_socket_change, resp.clientFd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL );
    if ( kevent( serverData.kqData.kq, & new_socket_change, 1, NULL, 0, NULL ) == ERROR )
        std::cerr << "kevent error" << std::endl;
}
