#include "utils/kqueueUtils.hpp"
#include "utils/dataStructs.hpp"
#include "server/server.hpp"
#include "http/HTTPResponseMessage.hpp"
#include <unistd.h>
#include "utils/webserv.hpp"

    //  * serverMap[ fd ].first = listeningSocket;
    //  * serverMap[ fd ].second = httpData;

void	disconnected(int fd, int *nbr_conn){
	std::cerr << "client disconnected" << std::endl;
	close( fd );
	(*nbr_conn)--;
}

void	takeRequest(webserv::serverData& serverData, int current_fd ) {
	serverData.requests[current_fd] += serverData.buf;
	if ( serverData.requests[current_fd].find("\r\n\r\n") != std::string::npos ){
		try{
			webserv::Request request( serverData.requests[current_fd] );
			std::cout << "made request object" << std::endl;
			HTTPResponseMessage response = handler( request, serverData.clientSockets[current_fd] );
			serverData.responses[current_fd] = response.toString();
			serverData.requests.erase(current_fd);
			printf( "  register respond event - %d\n", current_fd );
			struct kevent new_socket_change;
			EV_SET( &new_socket_change, current_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL );
			int ret = kevent( serverData.kqData.kq, &new_socket_change, 1, NULL, 0, NULL );
			if ( ret == ERROR ) {
            	perror( "  kqueue() failed" );
            	exit( EXIT_FAILURE );
        	}
		}
		catch( webserv::Request::IncorrectRequestException& e ){		// catches parsing errors from request
			std::cout << e.what() << std::endl;
		}
	}
}

void	processEvent(webserv::serverData& serverData, struct kevent& event){
	int current_fd = event.ident;
	if ( event.flags & EV_EOF ){		// check if it's an eof event, client disconnected
		disconnected( current_fd, &serverData.kqData.nbr_connections);
	} else if ( serverData.serverMap.find( current_fd ) != serverData.serverMap.end() )
		accepter( serverData.serverMap[current_fd] , serverData.kqData, serverData.clientSockets );
	else if ( serverData.responses.find( current_fd ) != serverData.responses.end() ){
		if ( responder(current_fd, serverData.responses) == FINISHED ) {
			struct kevent deregister_socket_change;
			EV_SET( &deregister_socket_change, current_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL );
			int ret = kevent( serverData.kqData.kq, &deregister_socket_change, 1, NULL, 0, NULL );
			if ( ret == ERROR ) {
            	perror( "  kqueue() failed" );
            	exit( EXIT_FAILURE );
        	}
		}
	} else {
		memset( serverData.buf, 0, serverData.buflen );	//clean struct
		int bytesread = recv( current_fd, serverData.buf, serverData.buflen, 0 );
		if ( bytesread < 0 ) {
			if ( errno != EWOULDBLOCK )
				std::cerr << "  recv() failed" << std::endl;
		}
		else if ( bytesread == 0 )
			std::cout << "  Connection closed" << std::endl; 
		else
			takeRequest( serverData, current_fd);
	}
}


int main( int argc, char **argv, char **env ) {
	if ( argc > 2 ){
		std::cout << "Incorrect program usage\n	RUN: ./webserv [configuration file]\n";
		return EXIT_FAILURE;
	}
	std::string config = argc == 1 ? "config.webserv" : argv[1];
	webserv::serverData serverData;

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
			processEvent(serverData, eventList[i]);
		}
	}
	return EXIT_SUCCESS;
}
