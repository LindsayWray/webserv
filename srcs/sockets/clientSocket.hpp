//
// Created by Kester kas De rooij on 3/14/22.
//

#ifndef WEBSERV_CLIENTSOCKET_HPP
#define WEBSERV_CLIENTSOCKET_HPP

#include "parentSocket.hpp"

namespace webserv {

    class clientSocket : public parentSocket {

    public:
        clientSocket( socketData input, int port );

        void connect_to_network( void );
    };

}

#endif //WEBSERV_CLIENTSOCKET_HPP
