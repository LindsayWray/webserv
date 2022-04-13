#include "webserv.hpp"
#include <iostream>
#include <string>

#define CHUNK_SIZE 1024

using namespace webserv;

bool webserv::responder( int fd, std::map<int, std::string>& responses ) {
    std::cout << "sending chunk of response" << std::endl;

    std::string chunk = responses[fd].substr( 0, CHUNK_SIZE );

    send( fd, chunk.c_str(), chunk.length(), 0 );

    if ( responses[fd].length() > CHUNK_SIZE ) {
        responses[fd] = responses[fd].substr( CHUNK_SIZE, responses[fd].length() );
        return NOT_FINISHED;
    } else {
        responses.erase( fd );
        return FINISHED;
    }
}