//
// Created by Kester kas De rooij on 3/15/22.
//

#ifndef WEBSERV_TESTSERVER_HPP
#define WEBSERV_TESTSERVER_HPP

#include "parentServer.hpp"
#include "../http/Request.hpp"
#include "../http/HTTPResponseMessage.hpp"

namespace webserv{
	class testServer : public parentServer{
	protected:
		char _buffer[20];
		int	_nb_of_conns;
		void _accepter();
		HTTPResponseMessage _handler( Request );
		void _responder(int fd, HTTPResponseMessage response);

	public:
		testServer( socketData d_socket, httpData d_http );
		void launch();
	};
}

#endif //WEBSERV_TESTSERVER_HPP
