#include "webserv.hpp"
#include <iostream>
#include <string>

#define CHUNK_SIZE 8192

using namespace webserv;


bool webserv::responder( int fd, std::map<int, std::pair<std::string, long> >& responses ) {
	static int count = 0;
    std::cout << "sending chunk of response" << ++count << std::endl;

    std::string chunk = responses[fd].first.substr( responses[fd].second, CHUNK_SIZE );

    int ret = send( fd, chunk.c_str(), chunk.length(), 0 );
	if (ret < 0) {
		responses.erase( fd );
		std::cerr << "failed to send response to client" << std::endl;
        return FINISHED;
	}

	responses[fd].second += ret;
	// std::cout << responses[fd].second << "/" << responses[fd].first.length() << std::endl;
    if ( responses[fd].second < responses[fd].first.length()) {
        return NOT_FINISHED;
    } else {
        responses.erase( fd );
        return FINISHED;
    }
}