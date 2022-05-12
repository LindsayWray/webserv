#include "parentSocket.hpp"

webserv::parentSocket::parentSocket( socketData input, int port ) {
    _address.sin_addr.s_addr = input.interface;
    _address.sin_family = input.domain;
    _address.sin_port = htons( port );
    _socket = socket( input.domain, input.service, input.protocol );
    test_connection( "Socket", _socket );
}

void webserv::parentSocket::test_connection( std::string funct, int connection ) {
    if ( connection < 0 ) {
        std::cerr << funct << " failure: " << strerror( errno ) << std::endl;
        exit( 1 );
    }
}

int webserv::parentSocket::get_sock( void ) {
    return this->_socket;
}

int webserv::parentSocket::get_connection( void ) {
    return this->_connection;
}

struct sockaddr_in webserv::parentSocket::get_address( void ) {
    return this->_address;
}
