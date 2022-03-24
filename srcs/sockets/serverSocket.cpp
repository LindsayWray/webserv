//
// Created by Kester kas De rooij on 3/14/22.
//

#include "serverSocket.hpp"

webserv::serverSocket::serverSocket( socketData input, int port )
		: parentSocket( input, port ) {
	set_sockt_opt( SO_REUSEADDR );
	set_nonblock();
	connect_to_network();
	test_connection( "Binding", _connection );
}

void webserv::serverSocket::connect_to_network( void ) {
	_connection = bind( _socket, (struct sockaddr *)&_address, sizeof(_address));
}

void webserv::serverSocket::set_nonblock( void ) {
	if ( fcntl( _socket, F_SETFL, O_NONBLOCK ) < 0 ) {
		std::cerr << "Fcntl() failed: " << strerror(errno) << std::endl;
		exit( EXIT_FAILURE );
	}
}

void webserv::serverSocket::set_sockt_opt( int option ) {
	int optval = 1;
	if ( setsockopt( _socket, SOL_SOCKET, option, (char *) &optval, sizeof( optval )) < 0 ) {
		std::cerr << "Setsockopt() failed: " << strerror(errno) << std::endl;
		exit( EXIT_FAILURE );
	}
	/*
	 * int setsockopt(int parentSocket, int level, int option_name, const void *option_value, socklen_t option_len)
	 *
	 * parentSocket = integer resulting from parentSocket()
	 * level = SOL_SOCKET -> manipulate options at parentSocket level
	 * option_name =
	 * -    SO_REUSEADDR = allow parentSocket descriptor to be reusable <sys/parentSocket.h>
	 * -    SO_REUSEPORT = allow ports to be reusable
	 * "The parameters option_value and option_len are used to access option values for setsockopt()"
	 */
}