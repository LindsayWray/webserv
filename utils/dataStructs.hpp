//
// Created by Kester kas De rooij on 3/15/22.
//

#ifndef WEBSERV_DATASTRUCTS_HPP
#define WEBSERV_DATASTRUCTS_HPP

#include <iostream>
#include <vector>
#include <map>

#define GET 0
#define POST 1
#define RESPONSE 2

namespace webserv{

	struct socketData{
		int domain;
		int service;
		int protocol;
		int port;
		u_long interface;
		int backlog;
		int worker_connections;
	};

	struct httpData{
	    std::vector<std::string> server_name;
        std::string root;
        int allowed_response[3];
	    std::map<int, std::string> error_page;
        std::map<int, std::string> redirect;
        bool autoindex;
	};

	struct readData{
		char* buf;
		int buflen;
		int bytesread;
	};
}

#endif //WEBSERV_DATASTRUCTS_HPP
