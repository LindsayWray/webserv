//
// Created by Kester kas De rooij on 3/14/22.
//
#include "serverObject.hpp"

webserv::serverObject::serverObject( int domain, int service, int protocol, int port, u_long interface, int backlog ){
	_socket = new listeningSocket( domain, service, protocol, port, interface, backlog );
}

webserv::serverObject::~serverObject( void ){
	delete _socket;
}