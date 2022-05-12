#include "webserv.hpp"

#define RESPONSES serverData.responses
#define REQUESTS serverData.requests
#define CLIENTS serverData.clientSockets
#define KQ serverData.kqData
#define SERVER_MAP serverData.serverMap
#define CGI_RESPONSES serverData.cgiResponses
#define ERROR_RESPONSE( code ) registerResponse(serverData, current_fd, errorResponse( CLIENTS[current_fd], code ))

using namespace webserv;

static void registerResponse( serverData& serverData, int current_fd, HTTPResponseMessage response ) {
    if ( response.getStatus() == HTTPResponseMessage::PAYLOAD_TOO_LARGE ){
        serverData.closeConnections.insert(current_fd);
        response.closeConnection();
    }
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
	if ( len > static_cast<int>(requestPath.size()) && !( locationPath.back() == "/" && len == static_cast<int>(requestPath.size()) + 1 ) )
		return false;
	if (locationPath.back() == "/" && len != 1)
		len--;
	// std::cout << "PATH: ";
	// for(int j = 0; j < static_cast<int>(requestPath.size()); j++){
	// 	std::cout << locationPath[j];
	// }
	// std::cout << std::endl;

	std::cout << "request PATH: ";
	for(int j = 0; j < static_cast<int>(requestPath.size()); j++){
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

int findRequestedLocation( httpData config, std::vector<std::string> path ) {
    for ( size_t i = 0; i < config.locations.size(); i++ ) {
		if ( pathIsMatch(path, config.locations[i].path) ) {
		    return i;
        }
    }
    return NOTFOUND;
}

static void disconnected( int fd, int& nbr_conn, serverData& serverData ) {
    REQUESTS.erase( fd );
    close( fd );
    nbr_conn--;
}

static bool isCGI( httpData serverblock, size_t default_i, Request request ){
    size_t pos = request.getPath().back().find(".py");
    locationData default_loc = serverblock.locations[default_i];

    if ( pos == std::string::npos )
        return false;
    if ( pos != request.getPath().back().size() - 3 )
        return false;
    if ( default_loc.CGI )
        return true;
    size_t len = default_loc.path.size();
    for ( size_t location_i = 0; location_i < serverblock.locations.size(); location_i++ ){
        if ( location_i == default_i )
            continue;
        if ( serverblock.locations[location_i].path.back() != "\\.py$" )
            continue;
        if ( default_loc.root != serverblock.locations[location_i].root )
            continue;
        for ( size_t param_i = 0; param_i < request.getPath().size(); param_i++ ) {
            if ( request.getPath()[len + param_i - 1] != serverblock.locations[location_i].cgi_param[param_i] )
                break;
            if ( serverblock.locations[location_i].cgi_param.size() - 1 == param_i )
                return true;
        }
    }
    return false;
}

static HTTPResponseMessage REDIRECT_handler( Request request, locationData request_location ) {
    HTTPResponseMessage response;
    std::string requestPath;
    std::string location = request_location.redirect.second;
    const size_t pos = location.find( "$uri" );
    if ( pos != std::string::npos ) {
        location.erase( pos, 4 );
        for ( size_t i = 0; i < request.getPath().size(); i++ )
            location.append( request.getPath()[i] );
    }
    response.addStatus( static_cast<HTTPResponseMessage::e_responseStatusCode>(request_location.redirect.first) )
            .addLength( 0 )
            .addBody( "" )
            .addLocation( location );
    return response;
}

static void takeRequest( serverData& serverData, int current_fd, int bytesread ) {
    if ( REQUESTS.find( current_fd ) == REQUESTS.end() )
        REQUESTS[current_fd] = Request( CLIENTS[current_fd].maxClientBody );    //creates new (empty) request in map
    Request& request = REQUESTS[current_fd];

    try { request.parseChunk( serverData.buf, bytesread ); }
    catch ( Request::MaxClientBodyException& e ) {
		std::cerr << e.what() << std::endl;
        return ERROR_RESPONSE( HTTPResponseMessage::PAYLOAD_TOO_LARGE );
    }
	catch ( webserv::Request::IncorrectRequestException &e ) {
		std::cerr << e.what() << std::endl;
		return ERROR_RESPONSE( HTTPResponseMessage::BAD_REQUEST );
	}
	catch ( webserv::Request::MethodNotAllowedException &e ) {
		std::cerr << e.what() << std::endl;
		return ERROR_RESPONSE( HTTPResponseMessage::METHOD_NOT_ALLOWED );
	}
        catch ( webserv::Request::NotImplementedException &e ) {
		std::cerr << e.what() << std::endl;
		return ERROR_RESPONSE( HTTPResponseMessage::NOT_IMPLEMENTED );
	}
    catch ( webserv::Request::VersionNotSupportedException &e ) {
		std::cerr << e.what() << std::endl;
		return ERROR_RESPONSE( HTTPResponseMessage::HTTP_VERSION_NOT_SUPPORTED );
	}
    catch ( webserv::Request::URITooLongException &e ) {
		std::cerr << e.what() << std::endl;
		return ERROR_RESPONSE( HTTPResponseMessage::URI_TOO_LONG );
	}

    if ( request.isComplete() ) {
        try {
            httpData serverblock = findServerBlock( serverData, request, current_fd );
			std::cout << "FIND LOCATION: " << std::endl;
            int location_index = findRequestedLocation( serverblock, request.getPath() );
            HTTPResponseMessage::e_responseStatusCode ret;
            if ( serverblock.locations[location_index].redirect.first != -1 )
                registerResponse( serverData, current_fd, REDIRECT_handler(request, CLIENTS[current_fd].locations[location_index]) );
            else if ( location_index == NOTFOUND )
                ERROR_RESPONSE( HTTPResponseMessage::INTERNAL_SERVER_ERROR );
            else {
                locationData location = serverblock.locations[location_index];
                if ( !location.allowed_response[request.getMethod()] )
                    registerResponse( serverData, current_fd, errorResponse( CLIENTS[current_fd], HTTPResponseMessage::METHOD_NOT_ALLOWED ) );
                else if ( location.CGI || isCGI( serverblock, location_index, request ) ) {
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
            if ( serverData.closeConnections.count(current_fd) ){
                disconnected( current_fd, KQ.nbrConnections, serverData );
                serverData.closeConnections.erase(current_fd);
            }
        }
    } else if ( CGI_RESPONSES.find( current_fd ) != CGI_RESPONSES.end() ) {
        responseFromCGI( serverData, current_fd );
        CGI_RESPONSES.erase( current_fd );
    } else {
        memset( serverData.buf, 0, serverData.buflen );
        int bytesread = recv( current_fd, serverData.buf, serverData.buflen, 0 );
        if ( bytesread < 0 )
			disconnected( current_fd, KQ.nbrConnections, serverData ); // removes client from kq
        else if ( bytesread == 0 )
			disconnected( current_fd, KQ.nbrConnections, serverData );
        else
            takeRequest( serverData, current_fd, bytesread );
    }

}

void webserv::kqueueFailure( int fd ) {
    std::cerr << "Kqueue Register Failure\n";
    close( fd );
}
