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
    RESPONSES[current_fd] = response.toString();
    REQUESTS.erase( current_fd );
    printf( "  register respond event - %d\n", current_fd );
    struct kevent new_socket_change;
    EV_SET( & new_socket_change, current_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL );
    if ( kevent( KQ.kq, & new_socket_change, 1, NULL, 0, NULL ) == ERROR )
        exit( EXIT_FAILURE );
}

static httpData findServerBlock( serverData serverData, Request request, int current_fd ) {
    std::pair<int, std::string> pairs( CLIENTS[current_fd].port, request.getHost() );
    if ( serverData.hostServername.find( pairs ) != serverData.hostServername.end() )
        return serverData.hostServername[pairs];
    return CLIENTS[current_fd];
}

static int findRequestedLocation( httpData config, std::vector<std::string> path ) {
    int len;
    for ( int i = 0; i < config.locations.size(); i++ ) {
        len = config.locations[i].path.size();
        if ( len > path.size() )
            continue;
        for ( int token = ( len - 1 ); token >= 0; token-- ) {
            if ( config.locations[i].path[token] != path[token] )
                break;
            if ( token == 0 )
                return i;
        }
    }
    return NOTFOUND;
}

static void disconnected( int fd, int& nbr_conn ) {
    std::cerr << "client disconnected" << std::endl;
    close( fd );
    nbr_conn--;
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

    if ( request.isComplete() ) {
        try {
            httpData serverblock = findServerBlock( serverData, request, current_fd );
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
        catch ( Request::IncorrectRequestException& e ) {        // catches parsing errors from request
            ERROR_RESPONSE( HTTPResponseMessage::BAD_REQUEST );
            std::cerr << e.what() << std::endl;
        }
    }
}

void webserv::processEvent( serverData& serverData, struct kevent& event ) {
    int current_fd = event.ident;

    if ( event.flags & EV_EOF ) {  // check if it's an eof event, client disconnected
        disconnected( current_fd, KQ.nbrConnections );
    } else if ( SERVER_MAP.find( current_fd ) != SERVER_MAP.end() ) {
        accepter( SERVER_MAP[current_fd], KQ, CLIENTS );
    } else if ( RESPONSES.find( current_fd ) != RESPONSES.end() ) {
        if ( responder( current_fd, RESPONSES ) == FINISHED ) {
            struct kevent deregister_socket_change;
            EV_SET( & deregister_socket_change, current_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL );
            int ret = kevent( KQ.kq, & deregister_socket_change, 1, NULL, 0, NULL );
            if ( ret == ERROR )
                exit( EXIT_FAILURE );
        }
    } else if ( CGI_RESPONSES.find( current_fd ) != CGI_RESPONSES.end() ) {
        if ( responseFromCGI( serverData, current_fd ) < 0 )
            exit( EXIT_FAILURE );
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


