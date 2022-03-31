//
// Created by Kester kas De rooij on 3/31/22.
//
#include "server.hpp"
#include <unistd.h>

std::string CGI( webserv::locationData *location, char** env ){
    HTTPResponseMessage response;
    static int i = 0;
    int ret = EXIT_FAILURE, status, pipes[2];
    int stdin_copy = dup(STDIN_FILENO);
    int stdout_copy = dup(STDOUT_FILENO);
    pid_t pid;
    std::string ret_str, line, reqPath = location->root;
    reqPath.append(location->cgi_param);
    char* args[3] = { strdup("/usr/bin/python"), strdup(reqPath.c_str()), NULL };
//    setenv("USER_NAME", "kas", 0);
//    std::cout << getenv("USER_NAME");
    pipe(pipes);
    pid = fork();
    if ( pid < 0 ){
        return NULL;
    } else if ( pid == 0 ){
        std::cerr << i++ << " iteration child" << std::endl;
        close( pipes[0] );
        dup2( pipes[1], 1 );
        close( pipes[1] );
        if ( (ret = execve( args[0], args, env )) < 0 )
            std::cerr << "file open went wrong" << std::endl;
        exit( ret );
    } else {
        waitpid( pid, &status, 0 );
        std::cerr << i++ << " iteration parent" << std::endl;
        close( pipes[1] );
        dup2( pipes[0], 0 );
        std::cin >> ret_str;
        while (std::getline( std::cin, line ) ) {
            ret_str.append("\n");
            ret_str.append(line);
        }
        close( pipes[0] );
        dup2(stdin_copy, 0);
        dup2(stdout_copy, 1);
        if (WIFEXITED(status))
            ret = WIFEXITED(status);
    }
    free( args[0] );
    free( args[1] );
    return ret_str;
}

void responseFromCGI(webserv::httpData* config, webserv::locationData *location, HTTPResponseMessage& response ) {
    std::string line;
    std::string body("");

    body = CGI( location, config->env );
    response.addStatus(HTTPResponseMessage::OK )
            .addLength(body.length())
            .addBody(body)
            .addType("text/plain");
}
