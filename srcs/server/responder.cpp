#include "server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/event.h>

#define CHUNK_SIZE 1024

bool responder( int fd, std::map<int, std::string> &responses ) {
    /* RESPONSE LOGGER */
    // std::cout << response.toString();

    /* FIRST ATTEMPT AT RESPONSE IMPL */
    std::cout << "sending chunk of response" << std::endl;

    std::string chunk = responses[fd].substr( 0, CHUNK_SIZE );

    //std::cout << "responseStr " << responseStr << std::endl;
    send( fd, chunk.c_str(), chunk.length(), 0 );

    if ( responses[fd].length() > CHUNK_SIZE ) {
        responses[fd] = responses[fd].substr( CHUNK_SIZE, responses[fd].length());
        return NOT_FINISHED;
    } else {
        responses.erase( fd );
        return FINISHED;
    }

    /* TEST WITH CONSTRUCTED RESPONSE */
    // std::ifstream outfile;
    // std::string line;
    // std::string body;
    // outfile.open("var/www/html/test.html");
    // while( std::getline( outfile, line ) ) {
    // 	body += line;
    // }
    // HTTPResponseMessage sut;
    // sut.addStatus(HTTPResponseMessage::NOT_FOUND)
    // 	.addType(HTTPResponseMessage::contentTypes.at("html"))
    // 	.addLength(body.length())
    // 	.addBody(body);
    // std::cout << sut.toString();
    // send( fd, sut.toString().c_str(), sut.toString().length(), 0);
}