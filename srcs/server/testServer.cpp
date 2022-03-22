//
// Created by Kester kas De rooij on 3/15/22.
//
#include "testServer.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>

webserv::testServer::testServer( socketData d_socket, httpData d_http ) : parentServer( d_socket, d_http ) {
	_kq = kqueue();
	struct	kevent listening_socket_change;
	EV_SET(&listening_socket_change, _socket->get_sock(), EVFILT_READ, EV_ADD, 0, 0, NULL);
	kevent(_kq, &listening_socket_change, 1, 0, 0, 0); // listen to listening socket

}

void webserv::testServer::_accepter() {
	struct sockaddr_in address = _socket->get_address();
	int new_sd = 0;
	// std::cout << "trying to accept: SD " << new_sd << " nbconn " << _nb_of_conns << " Ncon " << _Ncon << std::endl;
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

HTTPResponseMessage webserv::testServer::_handler( Request request ) {
	std::ifstream file;
	std::string line;
	std::string body("");
	HTTPResponseMessage response;

	std::string root = "./html";

	file.open(root + request.getPath());
	// file.seekg(0, std::ios::end); // This will set the fail bit if fileName is a directory (or do nothing if it is already set  
	if (file.good()) {
		std::cout << "File found " << root + request.getPath() << std::endl;
		while( std::getline( file, line ) ) {
			body += (line + '\n');
		}
		response.addStatus(HTTPResponseMessage::SUCCESS)
							.addTypeExt("text/html") // tells the client which datatype it can expect in the body
							.addLength(body.length())
							.addBody(body);
	} else {
		std::cout << "File not found " << root + request.getPath() << std::endl;

		body = "Not Found";
		response.addStatus(HTTPResponseMessage::NOT_FOUND)
					.addTypeExt("text/plain")
					.addLength(body.length())
					.addBody(body);
	}
	return response;
}

void webserv::testServer::_responder(int fd, HTTPResponseMessage response) {
	std::ifstream outfile;
	std::string line;

	std::cout << "sending response" << std::endl;

	outfile.open("index.html");
	while( std::getline( outfile, line ) ) {
		send( fd, line.c_str(), line.length(), 0 );
		send( fd, "\n", 1, 0 );
	}
	send( fd, "\n", 1, 0 );
	send( fd, "\n", 1, 0 );
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
							HTTPResponseMessage response = _handler( request);
							_responder(fd, response);
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
