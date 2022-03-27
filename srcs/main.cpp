#include "utils/kqueueUtils.hpp"
#include "utils/dataStructs.hpp"
#include "server/server.hpp"
#include "http/HTTPResponseMessage.hpp"
#include <unistd.h>

    //  * serverMap[ fd ].first = listeningSocket;
    //  * serverMap[ fd ].second = httpData;

int main( int argc, char **argv, char **env ) {
	if ( argc > 2 ){
		std::cout << "Incorrect program usage\n	RUN: ./webserv [configuration file]\n";
		return EXIT_FAILURE;
	}
	std::string config = argc == 1 ? "config.webserv" : argv[1];

    SERVER_MAP serverMap;
    webserv::kqConData kqData;
    if ( init_servers(serverMap, config, env, kqData) == ERROR )
        return EXIT_FAILURE;
    int nev, serv, current_fd;
    struct kevent* event = new struct kevent[MAX_EVENTS];
    kqData.nbr_connections = serverMap.size();
		std::cout << "NUMBER OF SERVER CONN: " << kqData.nbr_connections << std::endl;

	struct webserv::readData incoming;
	incoming.buflen = 1024;
	incoming.buf = new char[incoming.buflen + 1];
	incoming.buf[incoming.buflen] = '\0';

	std::map<int,std::string> requests;
	std::map<int,webserv::config_data*> clientSockets;


	while ( true ){
		std::cout << "Waiting on kqueue() events..." << std::endl;
		nev = kevent(kqData.kq, NULL, 0, event, MAX_EVENTS, NULL);
		if ( nev == ERROR ) {
			std::cerr << "  kqueue() failed" << std::endl;
			break;
		} else if ( nev == 0 ){
			std::cerr << "  kqueue() timed out.  End program." << std::endl;
			break;
		}
		for ( int i = 0; i < nev; i++ ){
			current_fd = event[i].ident;
			if ( event[i].flags & EV_EOF ){		// check if it's an eof event, client disconnected
				std::cerr << "Disconnected" << std::endl;
				close( current_fd );
				kqData.nbr_connections--;
			} else if ( serverMap.find( current_fd ) != serverMap.end() )
				accepter( serverMap[current_fd] , kqData, clientSockets );
			else {
				memset( incoming.buf, 0, incoming.buflen);	//clean struct
				incoming.bytesread = recv( current_fd, incoming.buf, incoming.buflen, 0 );
				if ( incoming.bytesread < 0 ) {
					if ( errno != EWOULDBLOCK )
						std::cerr << "  recv() failed" << std::endl;
				}
				else if ( incoming.bytesread == 0 )
					std::cout << "  Connection closed" << std::endl; 
				else {					
					requests[current_fd] += incoming.buf;
					if ( requests[current_fd].find("\r\n\r\n") != std::string::npos ){
						try{
							webserv::Request request( requests[current_fd] );
							std::cout << "made request object" << std::endl;
							HTTPResponseMessage response = handler( request, clientSockets[current_fd] );
							responder(current_fd, response);
							requests.erase(current_fd);
						}
						catch( webserv::Request::IncorrectRequestException& e ){		// catches parsing errors from request
							std::cout << e.what() << std::endl;
						}
					}
				}
			}
		}
	}
	return EXIT_SUCCESS;
}

