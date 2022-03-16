//
// Created by Kester kas De rooij on 3/15/22.
//

#ifndef WEBSERV_DATASTRUCTS_HPP
#define WEBSERV_DATASTRUCTS_HPP

#include <iostream>

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

	struct readData{
		char* buf;
		int buflen;
		int bytesread;
	};
}

#endif //WEBSERV_DATASTRUCTS_HPP
