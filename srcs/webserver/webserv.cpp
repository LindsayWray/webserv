#include "webserv.hpp"

#define RESPONSES serverData.responses
#define REQUESTS serverData.requests
#define CLIENTS serverData.clientSockets
#define KQ serverData.kqData
#define SERVER_MAP serverData.serverMap
#define CGI_RESPONSES serverData.cgiResponses
#define ERROR_RESPONSE( code ) registerResponse(serverData, current_fd, errorResponse( CLIENTS[current_fd], code ));

using namespace webserv;

static void registerResponse( serverData& serverData, int current_fd, HTTPResponseMessage response ) {
    REQUESTS.erase( current_fd );
    printf( "  register respond event - %d\n", current_fd );
    struct kevent new_socket_change;
    EV_SET( & new_socket_change, current_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL );
    if ( kevent( KQ.kq, & new_socket_change, 1, NULL, 0, NULL ) == ERROR )
        return kqueueFailure( current_fd );
	RESPONSES[current_fd] = std::make_pair(response.toString(), 0);
}

static httpData findServerBlock( serverData serverData, Request request, int current_fd ) {
    std::pair<int, std::string> pairs( CLIENTS[current_fd].port, request.getHost() );
    if ( serverData.hostServername.find( pairs ) != serverData.hostServername.end() )
        return serverData.hostServername[pairs];
    return CLIENTS[current_fd];
}

bool	pathIsMatch( std::vector<std::string> requestPath, std::vector<std::string> locationPath ){
	int len = locationPath.size();
	if ( len > requestPath.size() && !( locationPath.back() == "/" && len == requestPath.size() + 1 ) )
		return false;
	if (locationPath.back() == "/" && len != 1)
		len--;
	std::cout << "PATH: ";
	for(int j = 0; j < locationPath.size(); j++){
		std::cout << locationPath[j];
	}
	std::cout << std::endl;

	std::cout << "request PATH: ";
	for(int j = 0; j < requestPath.size(); j++){
		std::cout << requestPath[j];
	}
	std::cout << std::endl;
	
	for ( int token = ( len - 1 ); token >= 0; token-- ) {
		if ( locationPath[token] != requestPath[token] )
			break;
		if ( token == 0 )
			return true;
	}
	std::cout << "no match\n";
	return false;
}

static int findRequestedLocation( httpData config, std::vector<std::string> path ) {
    for ( int i = 0; i < config.locations.size(); i++ ) {
		if ( pathIsMatch(path, config.locations[i].path) )
			return i;
    }
    return NOTFOUND;
}

static void takeRequest( serverData& serverData, int current_fd, int bytesread ) {
    if ( REQUESTS.find( current_fd ) == REQUESTS.end() )
        REQUESTS[current_fd] = Request( CLIENTS[current_fd].maxClientBody );    //creates new (empty) request in map
    Request& request = REQUESTS[current_fd];

    try { request.parseChunk( serverData.buf, bytesread ); }
    catch ( Request::MaxClientBodyException& e ) {
        ERROR_RESPONSE( HTTPResponseMessage::PAYLOAD_TOO_LARGE );
        std::cerr << e.what() << std::endl;
    }
	catch ( webserv::Request::IncorrectRequestException &e ) {
		ERROR_RESPONSE( HTTPResponseMessage::BAD_REQUEST );
		std::cerr << e.what() << std::endl;
	}
	catch ( webserv::Request::MethodNotAllowedException &e ) {
		ERROR_RESPONSE( HTTPResponseMessage::METHOD_NOT_ALLOWED );
		std::cerr << e.what() << std::endl;
	}

    if ( request.isComplete() ) {
        try {
            httpData serverblock = findServerBlock( serverData, request, current_fd );
			std::cout << "FIND LOCATION: " << std::endl;
            int location_index = findRequestedLocation( serverblock, request.getPath() );
            HTTPResponseMessage::e_responseStatusCode ret;
            if ( location_index == NOTFOUND ) {
                ERROR_RESPONSE( HTTPResponseMessage::INTERNAL_SERVER_ERROR );
            } else {
                locationData location = serverblock.locations[location_index];
                if ( location.CGI ) {
                    ret = CGIRegister( location, serverData, current_fd, request );
                    if ( ret != HTTPResponseMessage::OK )
                        ERROR_RESPONSE( ret );
                } else {
                    HTTPResponseMessage response = handler( request, CLIENTS[current_fd], location );
                    registerResponse( serverData, current_fd, response );
                }
            }
        }
		catch ( webserv::Request::IncorrectRequestException &e ) {
			ERROR_RESPONSE( HTTPResponseMessage::BAD_REQUEST );
			std::cerr << e.what() << std::endl;
    	}
    }
}

static void disconnected( int fd, int& nbr_conn, serverData& serverData ) {
    std::cerr << "client disconnected" << std::endl;
	REQUESTS.erase( fd );
    close( fd );
    nbr_conn--;
}

void webserv::processEvent( serverData& serverData, struct kevent& event ) {
    int current_fd = event.ident;

    if ( event.flags & EV_EOF ) {
        disconnected( current_fd, KQ.nbrConnections, serverData );
    } else if ( SERVER_MAP.find( current_fd ) != SERVER_MAP.end() ) {
        accepter( SERVER_MAP[current_fd], KQ, CLIENTS );
    } else if ( RESPONSES.find( current_fd ) != RESPONSES.end() ) {
        if ( responder( current_fd, RESPONSES ) == FINISHED ) {
			RESPONSES.erase( current_fd );
            struct kevent deregister_socket_change;
            EV_SET( & deregister_socket_change, current_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL );
            if ( kevent( KQ.kq, & deregister_socket_change, 1, NULL, 0, NULL ) == ERROR )
                return kqueueFailure( current_fd );
        }
    } else if ( CGI_RESPONSES.find( current_fd ) != CGI_RESPONSES.end() ) {
        responseFromCGI( serverData, current_fd );
        CGI_RESPONSES.erase( current_fd );
    } else {
        memset( serverData.buf, 0, serverData.buflen );
        int bytesread = recv( current_fd, serverData.buf, serverData.buflen, 0 );
        if ( bytesread < 0 )
            std::cerr << "  recv() failed" << std::endl;
        else if ( bytesread == 0 )
            std::cout << "  Connection closed" << std::endl;
        else
            takeRequest( serverData, current_fd, bytesread );
    }
}

void webserv::kqueueFailure( int fd ) {
    std::cerr << "Kqueue Register Failure\n"; 
    close( fd );
}
