#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/event.h>
#include <map>
#include "../sockets/listeningSocket.hpp"
#include "../http/Request.hpp"
#include "../http/HTTPResponseMessage.hpp"

	void accept( webserv::listeningSocket* var, webserv::kqConData& kqData);
	HTTPResponseMessage handler( webserv::Request );
	void 				responder(int fd, HTTPResponseMessage response);

#endif
