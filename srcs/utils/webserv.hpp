#ifndef WEBSERV_WEBSERV_HPP
#define WEBSERV_WEBSERV_HPP


#include "../sockets/listeningSocket.hpp"

#define SERVER_MAP std::map<int, std::pair<webserv::listeningSocket*,webserv::httpData*> >

namespace webserv {
	

	typedef struct serverData {
		webserv::kqConData kqData;
		SERVER_MAP serverMap;
		
		std::map<int,std::string> requests;
		std::map<int,HTTPResponseMessage> responses;
		std::map<int,webserv::httpData*> clientSockets;

		char* buf;
		int buflen;
	} serverData;
};

#endif
