//
// Created by Kester kas De rooij on 3/31/22.
//
#include "server.hpp"
#include <unistd.h>
#include <sys/stat.h>

std::string CGI( webserv::locationData *location, char** env ){
    HTTPResponseMessage response;
    int ret = EXIT_FAILURE, status, pipes[2];
    pid_t pid;
    std::string ret_str, reqPath = location->root;
    struct stat sb{};
    reqPath.append(location->cgi_param);
    char* args[3] = { strdup("/usr/bin/python"), strdup(reqPath.c_str()), NULL };

    if ( pipe(pipes) != 0 )
        std::cerr << "pipe failed" << std::endl;
    pid = fork();
    if ( pid < 0 ){
        return NULL;
    } else if ( pid == 0 ){
        close( pipes[0] );
        dup2( pipes[1], STDOUT_FILENO );
        close( pipes[1] );
        if ( ( ret = execve( args[0], args, env ) ) < 0 )
            std::cerr << "file open went wrong" << std::endl;
        exit( ret );
    } else {
        waitpid( pid, &status, 0 );
        close( pipes[1] );
        fstat( pipes[0], &sb );
        ret_str.resize( sb.st_size );
        read( pipes[0], (char*)(ret_str.data()), sb.st_size );
        if ( WIFEXITED(status) )
            ret = WIFEXITED(status);
    }
    std::cout << ret_str;
    free( args[0] );
    free( args[1] );
    return ret_str;
}

HTTPResponseMessage responseFromCGI(webserv::httpData* config, webserv::locationData *location ) {
	HTTPResponseMessage response;
    std::string line;
    std::string body("");

    body = CGI( location, config->env );
    return response.addStatus(HTTPResponseMessage::OK )
            .addLength(body.length())
            .addBody(body)
            .addType("text/plain");
}
