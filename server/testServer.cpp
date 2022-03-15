//
// Created by Kester kas De rooij on 3/15/22.
//
#include "testServer.hpp"
#include <stdio.h>
#include <unistd.h>

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

void webserv::testServer::_handler() {

}

void webserv::testServer::_responder() {

}

void webserv::testServer::launch() {
	int rc, i, j;
	int timeout = 3 * 60 * 1000;
	_current = 1;
	int close_conn, end_server, compress_array;
	do {
		printf( "Waiting on poll()...\n" );
		rc = poll( _connections, _current, timeout );
		if ( rc < 0 ) {
			perror( "  poll() failed" );
			break;
		}
		if ( rc == 0 ) {
			printf( "  poll() timed out.  End program.\n" );
			break;
		}
		int current_size = _current;
		for ( i = 0; i < current_size; i++ ) {
			if ( _connections[i].revents == 0 )
				continue;
			if ( _connections[i].revents != POLLIN ) {
				printf( "  Error! revents = %d\n", _connections[i].revents );
				end_server = true;
				break;
			}
			if ( _connections[i].fd == _socket->get_sock() ) {
				_accepter();
			} else {
				printf( "  Descriptor %d is readable\n", _connections[i].fd );
				close_conn = false;
				do {
					_incoming.bytesread = recv( _connections[i].fd, _incoming.buf, _incoming.buflen, 0 );
					if ( _incoming.bytesread < 0 ) {
						if ( errno != EWOULDBLOCK ) {
							perror( "  recv() failed" );
							close_conn = true;
						}
						break;
					}
					if ( _incoming.bytesread == 0 ) {
						printf( "  Connection closed\n" );
						close_conn = true;
						break;
					}
					printf( "  %d bytes received\n", _incoming.bytesread );
					rc = send( _connections[i].fd, "hoi\n", 4, 0 );
					if ( rc < 0 ) {
						perror( "  send() failed" );
						close_conn = true;
						break;
					}
				} while ( true );
				if ( close_conn ) {
					close( _connections[i].fd );
					_connections[i].fd = -1;
					compress_array = true;
				}
			}  /* End of existing connection is readable             */
			std::cout << "going through" << std::endl;
		} /* End of loop through pollable descriptors              */
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
	} while ( end_server == false ); /* End of serving running.    */
	for ( i = 0; i < _current; i++ ) {
		if ( _connections[i].fd >= 0 )
			close( _connections[i].fd );
	}
}
