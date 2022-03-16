//
// Created by Kester kas De rooij on 3/15/22.
//
#include "testServer.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>

webserv::testServer::testServer( socketData input, int backlog, int worker_connections ) : parentServer( input, backlog, worker_connections ) {
	_connections->fd = _socket->get_sock();
	_connections->events = POLLIN;
}

void webserv::testServer::_accepter() {
	struct sockaddr_in address = _socket->get_address();
	int new_sd = 0;
	while ( new_sd != -1 && _current < _Ncon ) {
		new_sd = accept( _socket->get_sock(), (struct sockaddr *) &address, (socklen_t *) &address.sin_len);
		if ( new_sd < 0 ) {
			if (errno != EWOULDBLOCK) {
				std::cerr << errno << " " << strerror(errno) << std::endl;
				perror( "  accept() failed" ); // end server
			}
			return ;
		}
		printf( "  New incoming connection - %d\n", new_sd );
		_connections[_current].fd = new_sd;
		_connections[_current].events = POLLIN;
		_current++;
	}
}

void webserv::testServer::_handler( int fd ) {
	std::ifstream outfile;
	std::string line;
	outfile.open("../resp.html");
	while( getline( outfile, line ) ) {
		send( fd, line.c_str(), line.length(), 0 );
		send( fd, "\n", 1, 0 );
	}
	send( fd, "\n", 1, 0 );
	send( fd, "\n", 1, 0 );
}

void webserv::testServer::_responder() {

}

void webserv::testServer::launch() {
	int rc, i, j;
	int timeout = 3 * 60 * 1000;
	_current = 1;
	int close_conn, end_server, compress_array;
	do {
		timeout = 3 * 60 * 1000;
		std::cout << "Waiting on poll()..." << std::endl;
		rc = poll( _connections, _current, timeout );
		if ( rc < 0 ) {
			std::cerr << "  poll() failed" << std::endl;
			break;
		}
		if ( rc == 0 ) {
			std::cerr << "  poll() timed out.  End program." << std::endl;
			break;
		}
		int current_size = _current;
		for ( i = 0; i < current_size; i++ ) {
			if ( _connections[i].revents == 0 )
				continue;
			if ( _connections[i].revents != POLLIN ) {
				std::cerr << "  Error! revents = " << _connections[i].revents << std::endl;
				end_server = true;
				break;
			}
			if ( _connections[i].fd == _socket->get_sock() ) {
				_accepter();
			} else {
				std::cout << "  Descriptor " << _connections[i].fd << " is readable" << std::endl;
				close_conn = false;
				do {
					memset( _incoming.buf, 0, _incoming.buflen);
					_incoming.bytesread = recv( _connections[i].fd, _incoming.buf, _incoming.buflen, 0 );
					if ( _incoming.bytesread < 0 ) {
						if ( errno != EWOULDBLOCK ) {
							std::cerr << "  recv() failed" << std::endl;
							close_conn = true;
						}
						break;
					}
					if ( _incoming.bytesread == 0 ) {
						std::cout << "  Connection closed" << std::endl;
						close_conn = true;
						break;
					}
					std::cout << _incoming.bytesread << " bytes received" << std::endl;
					std::cout << _incoming.buf << std::endl;
					_handler( _connections[i].fd );
//					rc = send( _connections[i].fd, "hoi\n", 4, 0 );
//					if ( rc < 0 ) {
//						std::cerr << "  send() failed" << std::endl;
//						close_conn = true;
//						break;
//					}
				} while ( true );
				if ( close_conn ) {
					close( _connections[i].fd );
					_connections[i].fd = -1;
					compress_array = true;
				}
			}
		}
		if ( compress_array ) {
			compress_array = false;
			for ( i = 0; i < _current; i++ ) {
				if ( _connections[i].fd == -1 ) {
					for ( j = i; j < _current; j++ ) {
						_connections[j].fd = _connections[j + 1].fd;
					}
					i--;
					_current--;
				}
			}
		}
	} while ( end_server == false );
	for ( i = 0; i < _current; i++ ) {
		if ( _connections[i].fd >= 0 )
			close( _connections[i].fd );
	}
}
