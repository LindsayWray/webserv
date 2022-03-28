#include "utils/kqueueUtils.hpp"
#include "utils/dataStructs.hpp"
#include "server/server.hpp"
#include "http/HTTPResponseMessage.hpp"
#include <unistd.h>

    //  * serverMap[ fd ].first = listeningSocket;
    //  * serverMap[ fd ].second = httpData;

typedef struct serverData {
	webserv::kqConData kqData;
	SERVER_MAP serverMap;
	
	std::map<int,std::string> requests;
	std::map<int,HTTPResponseMessage> responses;
	std::map<int,webserv::config_data*> clientSockets;

	char* buf;
	int buflen;
} serverData; 


void	function(serverData& serverData, struct kevent& event){
	int current_fd = event.ident;
	if ( event.flags & EV_EOF ){		// check if it's an eof event, client disconnected
		std::cerr << "Disconnected" << std::endl;
		close( current_fd );
		kqData.nbr_connections--;
	} else if ( serverMap.find( current_fd ) != serverMap.end() )
		accepter( serverMap[current_fd] , kqData, clientSockets );
	else if ( responses.find( current_fd ) != responses.end() ){
		responder(current_fd, responses[current_fd]);
		responses.erase(current_fd);
		std::cerr << "Communication finished" << std::endl;
		close( current_fd );
		kqData.nbr_connections--;
	} else {
		memset( incoming.buf, 0, incoming.buflen );	//clean struct
		int bytesread = recv( current_fd, incoming.buf, incoming.buflen, 0 );
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
					responses[current_fd] = response;
					requests.erase(current_fd);
					printf( "  register respond event - %d\n", current_fd );
					struct kevent new_socket_change;
					EV_SET( &new_socket_change, current_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, NULL );
					kevent( kqData.kq, &new_socket_change, 1, NULL, 0, NULL );
				}
				catch( webserv::Request::IncorrectRequestException& e ){		// catches parsing errors from request
					std::cout << e.what() << std::endl;
				}
			}
		}
	}
}


int main( int argc, char **argv, char **env ) {
	if ( argc > 2 ){
		std::cout << "Incorrect program usage\n	RUN: ./webserv [configuration file]\n";
		return EXIT_FAILURE;
	}
	std::string config = argc == 1 ? "config.webserv" : argv[1];

	serverData serverData;

    if ( init_servers(serverData.serverMap, config, env, serverData.kqData) == ERROR )
        return EXIT_FAILURE;
    int nev, serv;
    struct kevent* eventList = new struct kevent[MAX_EVENTS];
    serverData.kqData.nbr_connections = serverData.serverMap.size();
	std::cout << "NUMBER OF SERVER CONN: " << serverData.kqData.nbr_connections << std::endl;

	serverData.buflen = 1024;
	serverData.buf = new char[serverData.buflen + 1];
	serverData.buf[serverData.buflen] = '\0';


	while ( true ){
		std::cout << "Waiting on kqueue() events..." << std::endl;
		nev = kevent(serverData.kqData.kq, NULL, 0, eventList, MAX_EVENTS, NULL);
		if ( nev == ERROR ) {
			std::cerr << "  kqueue() failed" << std::endl;
			break;
		} else if ( nev == 0 ){
			std::cerr << "  kqueue() timed out.  End program." << std::endl;
			break;
		}
		for ( int i = 0; i < nev; i++ ){
			function(serverData, eventList[i]);
		}
	}
	return EXIT_SUCCESS;
}

						