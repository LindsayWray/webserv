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
		serverData.kqData.nbr_connections--;
	} else if ( serverData.serverMap.find( current_fd ) != serverData.serverMap.end() )
		accepter( serverData.serverMap[current_fd] , serverData.kqData, serverData.clientSockets );
	else if ( serverData.responses.find( current_fd ) != serverData.responses.end() ){
		responder(current_fd, serverData.responses[current_fd]);
		serverData.responses.erase(current_fd);
		std::cerr << "Disconnected" << std::endl;
		close( current_fd );
		serverData.kqData.nbr_connections--;
	} else {
		memset( serverData.buf, 0, serverData.buflen );	//clean struct
		int bytesread = recv( current_fd, serverData.buf, serverData.buflen, 0 );
		if ( bytesread < 0 ) {
			if ( errno != EWOULDBLOCK )
				std::cerr << "  recv() failed" << std::endl;
		}
		else if ( bytesread == 0 )
			std::cout << "  Connection closed" << std::endl; 
		else {					
			serverData.requests[current_fd] += serverData.buf;
			if ( serverData.requests[current_fd].find("\r\n\r\n") != std::string::npos ){
				try{
					webserv::Request request( serverData.requests[current_fd] );
					std::cout << "made request object" << std::endl;
					HTTPResponseMessage response = handler( request, serverData.clientSockets[current_fd] );
					serverData.responses[current_fd] = response;
					serverData.requests.erase(current_fd);
					printf( "  register respond event - %d\n", current_fd );
					struct kevent new_socket_change;
					EV_SET( &new_socket_change, current_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, NULL );
					kevent( serverData.kqData.kq, &new_socket_change, 1, NULL, 0, NULL );
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

	printf("debug1\n");
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

						