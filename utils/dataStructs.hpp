//
// Created by Kester kas De rooij on 3/15/22.
//

#ifndef WEBSERV_DATASTRUCTS_HPP
#define WEBSERV_DATASTRUCTS_HPP

#include <iostream>
#include <vector>
#include <map>
#include <netinet/in.h>
#include <algorithm>

#define ERROR 1
#define SUCCES 0

#define GET 0
#define POST 1
#define DELETE 2

namespace webserv{

	struct socketData{
		int domain;
		int service;
		int protocol;
		std::vector<int> ports;
		u_long interface;
		int backlog;
		int worker_connections;

		socketData( void ){
			domain = AF_INET;
			service = SOCK_STREAM;
			protocol = 0;
			ports.push_back( 80 );
			interface = INADDR_ANY;
			backlog = 32;
			worker_connections = 0;
		}

		void addPort( int newPort ){ // start on port 80, if no port 80 is defined use other
			if ( std::find( ports.begin(), ports.end(), newPort ) == ports.end() )
				ports.push_back( newPort );
		}
	};

	struct locationData{
		std::string location;
		std::string root;
		int allowed_response[3]; // GET = 0, POST = 1, DELETE = 2
		bool autoindex;

		locationData( void ){
			root = "NONE";
			memset( allowed_response, 1, 3);
			autoindex = false;
		}
	};

	struct httpData{
	    std::vector<std::string> server_name;
		std::vector<std::string> index;
	    std::vector<std::pair<int, std::string>> error_page; // maybe map because of unique key's
        std::vector<std::pair<int, std::string>> redirect; // but vector is chronological which is nice
		std::vector<locationData> locations;
	};


	struct readData{
		char* buf;
		int buflen;
		int bytesread;
	};
}

#endif //WEBSERV_DATASTRUCTS_HPP
