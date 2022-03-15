//
// Created by Kester kas De rooij on 3/15/22.
//

#ifndef WEBSERV_TESTSERVER_HPP
#define WEBSERV_TESTSERVER_HPP

#include "parentServer.hpp"

namespace webserv{
	class testServer : public parentServer{
	protected:
		char _buffer[20];
		int	_current;
		void _accepter();
		void _handler();
		void _responder();

	public:
		testServer( socketData input, int backlog, int worker_connections );
		void launch();
	};
}

#endif //WEBSERV_TESTSERVER_HPP
