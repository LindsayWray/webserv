//
// Created by Kester kas De rooij on 3/15/22.
//
#include "testServer.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>

webserv::testServer::testServer( socketData input ) : parentServer( input ) {
	_kq = kqueue();
	struct	kevent listening_socket_change;
	EV_SET(&listening_socket_change, _socket->get_sock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	kevent(_kq, &listening_socket_change, 1, 0, 0, 0); // listen to listening socket
}

void webserv::testServer::_accepter() {
	struct sockaddr_in address = _socket->get_address();
	int new_sd = 0;
	while ( new_sd != -1 && _nb_of_conns < _Ncon ) {
		new_sd = accept( _socket->get_sock(), (struct sockaddr *) &address, (socklen_t *) &address.sin_len);
		if ( new_sd < 0 ) {
			if (errno != EWOULDBLOCK) {
				std::cerr << errno << " " << strerror(errno) << std::endl;
				perror( "  accept() failed" ); // end server
			}
			return ;
		}
		printf( "  New incoming connection - %d\n", new_sd );
		struct kevent new_socket_change;
		EV_SET(&new_socket_change, new_sd, EVFILT_READ, EV_ADD, 0, 0, NULL);
		kevent(_kq, &new_socket_change, 1, NULL, 0, NULL); // listen for events on newly created socket
		_nb_of_conns++;
	}
}

void webserv::testServer::_handler( int fd, Request request ) {
	std::ifstream outfile;
	std::string line;
	outfile.open("resp.html");
	while( std::getline( outfile, line ) ) {
		send( fd, line.c_str(), line.length(), 0 );
		send( fd, "\r\n", 2, 0 );
	}
	send( fd, "\r\n", 2, 0 );
	send( fd, "\r\n", 2, 0 );
}

void webserv::testServer::_responder() {
}

void webserv::testServer::launch() {
	int rc, i, j;
	int timeout = 3 * 60 * 1000;
	_nb_of_conns = 1;
	int nb_of_events = 32; // get good number here
	struct kevent evList[nb_of_events]; // create space for events

	int close_conn, end_server, compress_array;
	while(true) {
		std::cout << "Waiting on kqueue() events..." << std::endl;
		rc = kevent(_kq, 0, 0, evList, nb_of_events, 0);
		if ( rc < 0 ) {
			std::cerr << "  kqueue() failed" << std::endl;
			break;
		}
		if ( rc == 0 ) {
			std::cerr << "  kqueue() timed out.  End program." << std::endl;
			break;
		}
		for ( i = 0; i < rc; i++) {
			int fd = (int)evList[i].ident; // each event is identified by its socket fd

			if (evList[i].flags & EV_EOF) {
				printf("Disconnect\n");
				_nb_of_conns--;
				close(fd);
				// Socket is automatically removed from the kq by the kernel.
			}
			else if ( fd == _socket->get_sock() ) {
				_accepter();
			} else {
				close_conn = false;
				while ( true ) {
					memset( _incoming.buf, 0, _incoming.buflen);	//clean struct
					_incoming.bytesread = recv( fd, _incoming.buf, _incoming.buflen, 0 );
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
					_requests[fd] += _incoming.buf;
					if ( _requests[fd].find("\r\n\r\n") != std::string::npos ){
						try{
							webserv::Request request( _requests[fd] );
							std::cout << "made request object" << std::endl;
							_handler( fd, request) ;
						}
						catch( Request::IncorrectRequestException& e ){		// catches parsing errors from request 
							std::cout << e.what() << std::endl;
						}
						close_conn = true;
						break;
					}

				} 
			}
		}
	}
}
