#ifndef WEBSERV_PARENTSOCKET_HPP
#define WEBSERV_PARENTSOCKET_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "socketData.hpp"

namespace webserv {

    class parentSocket {

    protected:

        int _socket;
        int _connection;
        struct sockaddr_in _address;

    public:

        parentSocket( socketData input, int port );

        virtual void connect_to_network( void ) = 0;

        void test_connection( std::string, int );

        int get_sock( void );

        int get_connection( void );

        struct sockaddr_in get_address( void );
    };
}

#endif //WEBSERV_PARENTSOCKET_HPP
