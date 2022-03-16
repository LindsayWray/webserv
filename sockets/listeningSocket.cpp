//
// Created by Kester kas De rooij on 3/14/22.
//

#include "listeningSocket.hpp"

webserv::listeningSocket::listeningSocket( socketData input ) : serverSocket( input ) {
	_backlog = input.backlog;
	start_listening();
	test_connection( "Listening", _listening );
}

void webserv::listeningSocket::start_listening( void ) {
	_listening = listen( _socket, _backlog );
}

bool webserv::listeningSocket::is_listening( void ) {
	if ( _listening >= 0 )
		return true;
	return false;
}
