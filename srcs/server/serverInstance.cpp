//
// Created by Kester kas De rooij on 3/23/22.
//

#include "serverInstance.hpp"

webserv::serverInstance::serverInstance( int kq, socketData* d_socket, httpData* d_http ){
    _kq = kq;
    _nbrSockets = d_socket->ports.size();
    for (int i = 0; i < _nbrSockets; i++ )
        _socket.push_back( new listeningSocket(*d_socket, d_socket->ports[i] ) );
    _http = d_http;
}

webserv::serverInstance::~serverInstance( void ){
    for ( std::vector<webserv::listeningSocket*>::iterator it = _socket.begin(); it != _socket.end(); it++ )
        delete *it;
}

int webserv::serverInstance::getSocket(int index) {
    return _socket[index]->get_sock();
}

int webserv::serverInstance::getNbrSockets() {
    return _nbrSockets;
}

int webserv::serverInstance::checkIncoming( struct kevent incoming ){
    for ( int i = 0; i < _nbrSockets; i++ ){
        if ( incoming.ident == getSocket(i) )
            return i;
    }
    return ERROR;
}


