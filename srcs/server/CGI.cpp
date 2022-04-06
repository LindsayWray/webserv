//
// Created by Kester kas De rooij on 3/31/22.
//
#include "server.hpp"
#include <unistd.h>
#include <sys/stat.h>

pid_t pid;

std::string CGI_attempt( webserv::serverData &serverData ) {
    HTTPResponseMessage response;
    int ret = EXIT_FAILURE, status;
    std::string ret_str;
    struct stat sb{ };

    waitpid( pid, &status, 0 );
    fstat( serverData.CGI, &sb );
    ret_str.resize( sb.st_size );
    read( serverData.CGI, ( char * ) ( ret_str.data()), sb.st_size );
    close( serverData.CGI );
    if ( WIFEXITED( status ))
        ret = WIFEXITED( status );
    return ret_str;
}

int CGI_register( webserv::locationData location, webserv::serverData &serverData, char **env, int client_socket_fd ) {
    int pipes[2], ret;
    std::string ret_str, reqPath = location.root;

    reqPath.append( location.cgi_param );
    char *args[3] = { strdup( "/usr/bin/python" ), strdup( reqPath.c_str()), NULL };

    if ( pipe( pipes ) != 0 )
        std::cerr << "pipe failed" << std::endl;
    // serverData.CGI = pipes[0];
	serverData.cgi_repsonses[pipes[0]] = std::make_pair(client_socket_fd, "");

    struct kevent new_socket_change;
    EV_SET( &new_socket_change, pipes[0], EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL );
    int ret2 = kevent( serverData.kqData.kq, &new_socket_change, 1, NULL, 0, NULL );
    if ( ret2 == ERROR )
        return -1;

    pid = fork();
    if ( pid < 0 ) {
        return -1;
    } else if ( pid == 0 ) {
        close( pipes[0] );
        dup2( pipes[1], STDOUT_FILENO );
        close( pipes[1] );
        if (( ret = execve( args[0], args, env )) < 0 )
            std::cerr << "file open went wrong" << std::endl;
        exit( ret );
    }
    free( args[0] );
    free( args[1] );
    return 0;
}

int responseFromCGI( webserv::serverData &serverData ) {
    HTTPResponseMessage response;
    std::string line;
    std::string body( "" );

    body = CGI_attempt( serverData );
    response.addStatus( HTTPResponseMessage::OK )
            .addLength( body.length())
            .addBody( body )
            .addType( "text/plain" );
    serverData.responses[serverData.current_fd] = response.toString();
    serverData.requests.erase( serverData.current_fd );
    printf( "  register CGI respond event - %d\n", serverData.current_fd );
    struct kevent new_socket_change;
    EV_SET( &new_socket_change, serverData.current_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL );
    int ret = kevent( serverData.kqData.kq, &new_socket_change, 1, NULL, 0, NULL );
    if ( ret == ERROR )
        return -1;
    return 0;
}
 