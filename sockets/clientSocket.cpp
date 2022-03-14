//
// Created by Kester kas De rooij on 3/14/22.
//

#include "clientSocket.hpp"

webserv::clientSocket::clientSocket( int domain, int service, int protocol, int port, u_long interface )
		: parentSocket( domain, service, protocol, port, interface ) {
	connect_to_network();
	test_connection( "Connecting", _connection );
}

void webserv::clientSocket::connect_to_network( void ) {
	_connection = connect( _socket, (struct sockaddr *)&_address, sizeof(_address));
}
