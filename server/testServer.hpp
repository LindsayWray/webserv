//
// Created by Kester kas De rooij on 3/15/22.
//

#ifndef WEBSERV_TESTSERVER_HPP
#define WEBSERV_TESTSERVER_HPP

#include "parentServer.hpp"
#include "../http/Request.hpp"

namespace webserv{
	class testServer : public parentServer{
	protected:
		char _buffer[20];
		int	_nb_of_conns;
		void _accepter();
		void _handler( int, Request );
		void _responder();

	public:
		testServer( socketData input );
		void launch();
	};
}

#endif //WEBSERV_TESTSERVER_HPP
