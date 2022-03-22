//
// Created by Kester kas De rooij on 3/14/22.
//
#include "parentServer.hpp"

webserv::parentServer::parentServer( socketData d_socket, httpData d_http ) : _http(d_http) {
	_socket = new listeningSocket( d_socket );
	_incoming.buflen = 1024;
	_incoming.buf = new char[_incoming.buflen];
	_Ncon = d_socket.worker_connections;
}

webserv::parentServer::~parentServer( void ){
	delete _socket;
}

webserv::listeningSocket* webserv::parentServer::get_socket( void ) {
	return _socket;
}

