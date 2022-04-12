//
// Created by Kester kas De rooij on 4/6/22.
//

#include "webserv.hpp"

#define CLIENT( fd ) serverData.clientSockets[fd]
#define RESPONSES serverData.responses
#define REQUESTS serverData.requests
#define SERVER_MAP serverData.serverMap
#define KQ serverData.kqData
#define CGI_RESPONSE serverData.cgi_responses
#define ERROR_RESPONSE( fd, co ) registerResponse( serverData, fd, errorResponse( CLIENT( fd ), co ))
#define IS_FD( mac, fd ) mac.find( fd ) != mac.end()


void registerResponse( webserv::serverData &serverData, int current_fd, HTTPResponseMessage response ) {
    serverData.responses[current_fd] = response.toString();
    serverData.requests.erase( current_fd );
    printf( "  register respond event - %d\n", current_fd );
    struct kevent new_socket;
    EV_SET( &new_socket, current_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL );
    if ( kevent( KQ.kq, &new_socket, 1, NULL, 0, NULL ) == ERROR )
        exit( EXIT_FAILURE );
}

void webserv::processEvent( webserv::serverData &serverData, struct kevent &event ) {
    int current_fd = event.ident;

    if ( event.flags & EV_EOF ) {  // check if it's an eof event, client disconnected
        disconnected( current_fd, KQ.nbr_connections );
    } else if ( IS_FD( SERVER_MAP, current_fd ) ) {
        accepter( SERVER_MAP[current_fd], KQ, serverData.clientSockets );
    } else if ( IS_FD( RESPONSES, current_fd ) ) {
        if ( responder( current_fd, RESPONSES ) == FINISHED ) {
            struct kevent deregister;
            EV_SET( &deregister, current_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL );
            if ( kevent( KQ.kq, &deregister, 1, NULL, 0, NULL ) == ERROR )
                exit( EXIT_FAILURE );
        }
    } else if ( IS_FD( CGI_RESPONSE, current_fd ) ) {
        if ( responseFromCGI( serverData, current_fd ) < 0 )
            exit( EXIT_FAILURE );
        CGI_RESPONSE.erase( current_fd );
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

void webserv::takeRequest( webserv::serverData &serverData, int current_fd, int bytesread ) {
    if ( IS_FD( REQUESTS, current_fd ) )
        REQUESTS[current_fd] = webserv::Request(
                CLIENT( current_fd )->max_client_body_size );    //creates new (empty) request in map
    webserv::Request &request = REQUESTS[current_fd];

    try { request.parseChunk( serverData.buf, bytesread ); }
    catch ( webserv::Request::MaxClientBodyException &e ) {
        ERROR_RESPONSE( current_fd, HTTPResponseMessage::PAYLOAD_TOO_LARGE );
        std::cerr << e.what() << std::endl;
    }

    if ( request.isComplete()) {
        try {
            int location_index = findRequestedLocation( CLIENT( current_fd ), request.getPath());
            HTTPResponseMessage::e_responseStatusCode ret;
            if ( location_index == NOTFOUND ) {
                ERROR_RESPONSE( current_fd, HTTPResponseMessage::INTERNAL_SERVER_ERROR );
            } else {
                webserv::locationData location = CLIENT( current_fd )->locations[location_index];
                if ( location.CGI ) {
                    ret = CGI_register( location, serverData, CLIENT( current_fd )->env, current_fd, request );
                    if ( ret != HTTPResponseMessage::OK )
                        ERROR_RESPONSE( current_fd, ret );
                } else {
                    HTTPResponseMessage response = handler( request, CLIENT( current_fd ), location );
                    registerResponse( serverData, current_fd, response );
                }
            }
        }
        catch ( webserv::Request::IncorrectRequestException &e ) {        // catches parsing errors from request
            ERROR_RESPONSE( current_fd, HTTPResponseMessage::BAD_REQUEST );
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


