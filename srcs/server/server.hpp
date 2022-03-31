#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/event.h>
#include <map>
#include "../sockets/listeningSocket.hpp"
#include "../http/Request.hpp"
#include "../http/HTTPResponseMessage.hpp"

	void accepter( std::pair<webserv::listeningSocket*,webserv::httpData*>& serverPair, 
		webserv::kqConData& kqData,std::map<int,webserv::httpData*>& clientSockets);
	HTTPResponseMessage handler( webserv::Request request, webserv::httpData* config );
	void 				responder(int fd, HTTPResponseMessage response);
    void responseFromCGI(webserv::httpData* config, webserv::locationData *location, HTTPResponseMessage& response );

#endif
