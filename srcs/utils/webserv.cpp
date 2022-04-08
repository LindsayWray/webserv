//
// Created by Kester kas De rooij on 4/6/22.
//

#include "webserv.hpp"

//  * serverMap[ fd ].first = listeningSocket;
//  * serverMap[ fd ].second = httpData;

void	registerResponse(webserv::serverData& serverData, int current_fd, HTTPResponseMessage& response){
	serverData.responses[current_fd] = response.toString();
	serverData.requests.erase( current_fd );
	printf( "  register respond event - %d\n", current_fd );
	struct kevent new_socket_change;
	EV_SET( &new_socket_change, current_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL );
	int ret = kevent( serverData.kqData.kq, &new_socket_change, 1, NULL, 0, NULL );
	if ( ret == ERROR ) {
		perror( "  kqueue() failed" );
		exit( EXIT_FAILURE );
	}
}

void webserv::processEvent( webserv::serverData& serverData, struct kevent& event ) {
    int current_fd = event.ident;

    if ( event.flags & EV_EOF ) {  // check if it's an eof event, client disconnected
        disconnected( current_fd, serverData.kqData.nbr_connections );
    } else if ( serverData.serverMap.find( current_fd ) != serverData.serverMap.end()) {
        accepter( serverData.serverMap[current_fd], serverData.kqData, serverData.clientSockets );
    } else if ( serverData.responses.find( current_fd ) != serverData.responses.end()) {
        if ( responder( current_fd, serverData.responses ) == FINISHED ) {
            struct kevent deregister_socket_change;
            EV_SET( &deregister_socket_change, current_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL );
            int ret = kevent( serverData.kqData.kq, &deregister_socket_change, 1, NULL, 0, NULL );
            if ( ret == ERROR )
                exit( EXIT_FAILURE );
        }
    } else if ( serverData.cgi_responses.find( current_fd ) != serverData.cgi_responses.end() ) {
        if ( responseFromCGI( serverData, current_fd ) < 0 )
            exit( EXIT_FAILURE );
    } else {
        memset( serverData.buf, 0, serverData.buflen );    //clean struct
        int bytesread = recv( current_fd, serverData.buf, serverData.buflen, 0 );
        if ( bytesread < 0 )
            std::cerr << "  recv() failed" << std::endl;
        else if ( bytesread == 0 )
            std::cout << "  Connection closed" << std::endl;
        else
            takeRequest( serverData, current_fd, bytesread );
    }
}

void webserv::takeRequest( webserv::serverData &serverData, int current_fd, int bytesread ) {
    serverData.requests[current_fd].append(serverData.buf, bytesread);

    //std::cout << "LEN " << serverData.buflen << " " << bytesread << std::endl;
    if (bytesread == serverData.buflen)
        return; 

    if ( serverData.requests[current_fd].find( "\r\n\r\n" ) != std::string::npos ) {
        try {
            webserv::Request request( serverData.requests[current_fd] );
            std::cout << "made request object" << std::endl;
            int location_index = findRequestedLocation( serverData.clientSockets[current_fd], request.getPath());
            if ( location_index == NOTFOUND );
                // TODO:: do something
            webserv::locationData location = serverData.clientSockets[current_fd]->locations[location_index];
            if ( location.CGI ) {
                if ( CGI_register( location, serverData, serverData.clientSockets[current_fd]->env, current_fd ) < 0 )
                    std::cerr << "we have to handle this error" << std::endl;
            } else {
                HTTPResponseMessage response = handler( request, serverData.clientSockets[current_fd], location );
				registerResponse(serverData, current_fd, response);
            }
        }
        catch ( webserv::Request::IncorrectRequestException &e ) {        // catches parsing errors from request
			HTTPResponseMessage response;
			std::string body = serverData.clientSockets[current_fd]->error_page[400];
			    response.addStatus( HTTPResponseMessage::BAD_REQUEST )
            		.addLength( body.size() )
            		.addBody( body );
			registerResponse(serverData, current_fd, response);
            std::cerr << e.what() << std::endl;
        }
   }
}

int webserv::findRequestedLocation( webserv::httpData *config, std::vector<std::string> path ) {
    int len;
    for ( int i = 0; i < config->locations.size(); i++ ) {
        len = config->locations[i].path.size();
        if ( len > path.size())
            continue;
        for ( int token = ( len - 1 ); token >= 0; token-- ) {
            if ( config->locations[i].path[token] != path[token] )
                break;
            if ( token == 0 )
                return i;
        }
    }
    return NOTFOUND;
}

void webserv::disconnected( int fd, int &nbr_conn ) {
    std::cerr << "client disconnected" << std::endl;
    close( fd );
    nbr_conn--;
}


