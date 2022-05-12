#ifndef WEBSERV_SOCKETDATA_HPP
#define WEBSERV_SOCKETDATA_HPP

#include <vector>
#include <iostream>
#include <netinet/in.h>

namespace webserv {
    struct socketData {
        int domain;
        int service;
        int protocol;
        std::vector<int> ports;
        u_long interface;
        int backlog;
        int workerConnections;

        socketData( void );

        void addPort( int newPort );
    };
}

#endif //WEBSERV_SOCKETDATA_HPP
