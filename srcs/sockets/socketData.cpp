#include "socketData.hpp"

webserv::socketData::socketData( void ) {
    domain = AF_INET;
    service = SOCK_STREAM;
    protocol = 0;
    interface = INADDR_ANY;
    backlog = 1024;
    workerConnections = 1024;
}

void webserv::socketData::addPort( int newPort ) { // start on port 80, if no port 80 is defined use other
    if ( std::find( ports.begin(), ports.end(), newPort ) == ports.end() )
        ports.push_back( newPort );
}
