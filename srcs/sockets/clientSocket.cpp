#include "clientSocket.hpp"

webserv::clientSocket::clientSocket( socketData input, int port ) : parentSocket( input, port ) {
    connect_to_network();
    test_connection( "Connecting", _connection );
}

void webserv::clientSocket::connect_to_network( void ) {
    _connection = connect( _socket, ( struct sockaddr* ) & _address, sizeof( _address ) );
}
