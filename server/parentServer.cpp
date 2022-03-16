//
// Created by Kester kas De rooij on 3/14/22.
//
#include "parentServer.hpp"

webserv::parentServer::parentServer( socketData input, int backlog, int worker_connections ){
	_socket = new listeningSocket( input, backlog );
	_incoming.buflen = 1024;
	_incoming.buf = new char[_incoming.buflen];
	_Ncon = worker_connections;
	_connections = new struct pollfd[_Ncon];
}

webserv::parentServer::~parentServer( void ){
	delete _socket;
	delete _connections;
}

webserv::listeningSocket* webserv::parentServer::get_socket( void ) {
	return _socket;
}

struct pollfd* webserv::parentServer::get_server( void ) {
	return _connections;
}
