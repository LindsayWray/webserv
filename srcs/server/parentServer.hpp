//
// Created by Kester kas De rooij on 3/14/22.
//
#ifndef WEBSERV_PARENTSERVER_HPP
#define WEBSERV_PARENTSERVER_HPP

#include <sys/event.h>
#include <map>
#include "../sockets/listeningSocket.hpp"
#include "../http/Request.hpp"
#include "../http/HTTPResponseMessage.hpp"

namespace webserv{
	class parentServer{
	protected:
		int _kq;
		httpData _http;
		listeningSocket* _socket;
		readData _incoming;
		int _Ncon;
		std::map<int,std::string> _requests;
		virtual void _accepter() = 0;
		virtual HTTPResponseMessage _handler( Request ) = 0;
		virtual void _responder(int fd, HTTPResponseMessage response) = 0;

	public:
		parentServer( socketData d_socket, httpData d_http );
		virtual ~parentServer();
		virtual void launch() = 0;
		listeningSocket* get_socket();
	};
}

#endif //WEBSERV_PARENTSERVER_HPP
