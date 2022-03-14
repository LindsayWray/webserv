//
// Created by Kester kas De rooij on 3/14/22.
//

#include "listeningSocket.hpp"

webserv::listeningSocket::listeningSocket( int domain, int service, int protocol, int port, u_long interface, int backlog )
		: serverSocket( domain, service, protocol, port, interface ) {
	_backlog = backlog;
	start_listening();
	test_connection( "Listening", _listening );
}

void webserv::listeningSocket::start_listening( void ) {
	_listening = listen( _socket, _backlog );
}
