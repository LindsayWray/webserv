#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/event.h>
#include <map>
#include "../sockets/listeningSocket.hpp"
#include "../http/Request.hpp"
#include "../http/HTTPResponseMessage.hpp"

	void accepter( std::pair<webserv::listeningSocket*,webserv::httpData*>& serverPair, 
		webserv::kqConData& kqData,std::map<int,webserv::config_data*>& clientSockets);
	HTTPResponseMessage handler( webserv::Request request, webserv::config_data* config );
	void 				responder(int fd, HTTPResponseMessage response);

#endif
