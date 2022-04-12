#include "webserv.hpp"
#include "printFormatting.hpp"
#define RESPONSES serverData.responses
#define REQUESTS serverData.requests
#define CLIENTS serverData.clientSockets
#define KQ serverData.kqData
#define SERVER_MAP serverData.serverMap
#define CGI_RESPONSES serverData.cgi_responses
#define ERROR_RESPONSE( code ) registerResponse(serverData, current_fd, errorResponse( CLIENTS[current_fd], code ));

void webserv::registerResponse( webserv::serverData &serverData, int current_fd, HTTPResponseMessage response ) {
    REQUESTS.erase( current_fd );
    printf( "  register respond event - %d\n", current_fd );
    struct kevent new_socket_change;
    EV_SET( &new_socket_change, current_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL );
    if ( kevent( KQ.kq, &new_socket_change, 1, NULL, 0, NULL ) == ERROR ){
        return kqueueFailure( current_fd );
    }
    RESPONSES[current_fd] = response.toString();
}

void webserv::processEvent( webserv::serverData &serverData, struct kevent &event ) {
    int current_fd = event.ident;

    if ( event.flags & EV_EOF ) {
        disconnected( current_fd, KQ.nbr_connections );
    } else if ( SERVER_MAP.find( current_fd ) != SERVER_MAP.end()) {
        accepter( SERVER_MAP[current_fd], KQ, CLIENTS );
    } else if ( RESPONSES.find( current_fd ) != RESPONSES.end()) {
        if ( responder( current_fd, RESPONSES ) == FINISHED ) {
            struct kevent deregister_socket_change;
            EV_SET( &deregister_socket_change, current_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL );
            if ( kevent( KQ.kq, &deregister_socket_change, 1, NULL, 0, NULL ) == ERROR )
                return kqueueFailure( current_fd );
            RESPONSES.erase( current_fd );
        }
    } else if ( CGI_RESPONSES.find( current_fd ) != CGI_RESPONSES.end()) {
        if ( responseFromCGI( serverData, current_fd ) < 0 )
            return ;
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

static webserv::httpData* findServerBlock( webserv::serverData serverData, webserv::Request request, int current_fd ){
    std::pair<int,std::string> pairs( CLIENTS[current_fd]->port, request.getHost() );
    if ( serverData.host_servername.find(pairs) != serverData.host_servername.end() )
        return serverData.host_servername[pairs];
    return CLIENTS[current_fd];
}

void webserv::takeRequest( webserv::serverData &serverData, int current_fd, int bytesread ) {
    if ( REQUESTS.find( current_fd ) == REQUESTS.end())
        REQUESTS[current_fd] = webserv::Request( CLIENTS[current_fd]->max_client_body_size );    //creates new (empty) request in map
    webserv::Request &request = REQUESTS[current_fd];

    try { request.parseChunk( serverData.buf, bytesread ); }
    catch ( webserv::Request::MaxClientBodyException &e ) {
        ERROR_RESPONSE( HTTPResponseMessage::PAYLOAD_TOO_LARGE );
        std::cerr << e.what() << std::endl;
    }
    catch ( webserv::Request::IncorrectRequestException &e ) {
        ERROR_RESPONSE( HTTPResponseMessage::BAD_REQUEST );
        std::cerr << e.what() << std::endl;
    }

    if ( request.isComplete()) {
        try {
            webserv::httpData* serverblock = findServerBlock( serverData, request, current_fd );
            int location_index = findRequestedLocation( serverblock, request.getPath());
            HTTPResponseMessage::e_responseStatusCode ret;
            if ( location_index == NOTFOUND ) {
                ERROR_RESPONSE( HTTPResponseMessage::INTERNAL_SERVER_ERROR );
            } else {
                webserv::locationData location = serverblock->locations[location_index];
                if ( location.CGI ) {
                    ret = CGI_register( location, serverData, current_fd, request );
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

void webserv::kqueueFailure( int fd ) {
    std::cerr << "Kqueue Register Failure\n"; 
    close( fd );
}