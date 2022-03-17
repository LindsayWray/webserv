//
// Created by Kester kas De rooij on 3/14/22.
//

#ifndef WEBSERV_PARENTSERVER_HPP
#define WEBSERV_PARENTSERVER_HPP
#include <sys/poll.h>
#include <map>
#include "../sockets/listeningSocket.hpp"

namespace webserv{
	class parentServer{
	protected:
		listeningSocket* _socket;
		struct pollfd*	_connections;
		readData _incoming;
		int _Ncon;
		std::map<int,std::string> _requests;
		virtual void _accepter() = 0;
		virtual void _handler( int, Request ) = 0;
		virtual void _responder() = 0;

	public:
		parentServer( socketData input );
		virtual ~parentServer();
		virtual void launch() = 0;
		listeningSocket* get_socket();
		struct pollfd* get_server();
	};
}

#endif //WEBSERV_PARENTSERVER_HPP
