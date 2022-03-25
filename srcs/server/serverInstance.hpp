//
// Created by Kester kas De rooij on 3/23/22.
//

#ifndef WEBSERV_SERVERINSTANCE_HPP
#define WEBSERV_SERVERINSTANCE_HPP

#include <sys/event.h>
#include <map>
#include "../sockets/listeningSocket.hpp"
#include "../http/Request.hpp"
#include "../http/HTTPResponseMessage.hpp"
#include "../http/Request.hpp"
#include "../http/HTTPResponseMessage.hpp"

namespace webserv{
    class serverInstance{
    private:
        int _nbrSockets;
        int _kq;
        httpData* _http;
        std::vector<listeningSocket*> _socket;

    public:
        serverInstance( int kq, socketData* d_socket, httpData* d_http );
        ~serverInstance( void );
        int getNbrSockets( void );
        int getSocket( int index );
        int checkIncoming( struct kevent incoming );

        void accept( int kq, struct kevent event, int** connections );
    };
}

#endif //WEBSERV_SERVERINSTANCE_HPP
