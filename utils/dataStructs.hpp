//
// Created by Kester kas De rooij on 3/15/22.
//

#ifndef WEBSERV_DATASTRUCTS_HPP
#define WEBSERV_DATASTRUCTS_HPP

#include <iostream>
#include <vector>
#include <map>
#include <netinet/in.h>

#define SUCCES 0
#define ERROR 1

#define GET 0
#define POST 1
#define DELETE 2

namespace webserv{

	struct socketData{
		int domain;
		int service;
		int protocol;
		int* ports;
		u_long interface;
		int backlog;
		int worker_connections;

		socketData( void ){
			domain = AF_INET;
			service = SOCK_STREAM;
			protocol = 0;
			ports = NULL;
			interface = INADDR_ANY;
			backlog = 32;
			worker_connections = 0;
		}

		~socketData( void ){ // this deconstructor gives sigabort when deleting ports but i dont know why
//			if ( sizeof(ports) > 0 )
//				delete ports;
		}

		void addPort( int newPort ){
			int i = 0;
			int len = ports ? sizeof(ports) : 0;
			int* tmp = new int[len + 1];

			for (; i < len; i++)
				tmp[i] = ports[i];
			tmp[i] = newPort;
			delete ports;
			ports = tmp;
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
