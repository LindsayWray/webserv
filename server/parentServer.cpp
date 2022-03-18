//
// Created by Kester kas De rooij on 3/14/22.
//
#include "parentServer.hpp"

webserv::parentServer::parentServer( socketData input ){
	_socket = new listeningSocket( input );
	_incoming.buflen = 1024;
	_incoming.buf = new char[_incoming.buflen];
	_Ncon = input.worker_connections;
}

webserv::parentServer::~parentServer( void ){
	delete _socket;
}

webserv::listeningSocket* webserv::parentServer::get_socket( void ) {
	return _socket;
}

