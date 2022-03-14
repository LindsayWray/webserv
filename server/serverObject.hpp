//
// Created by Kester kas De rooij on 3/14/22.
//

#ifndef WEBSERV_SERVEROBJECT_HPP
#define WEBSERV_SERVEROBJECT_HPP

#include "../sockets/listeningSocket.hpp"

namespace webserv{
	class serverObject{
	private:
		listeningSocket* _socket;
		virtual void _accepter() = 0;
		virtual void _handler() = 0;
		virtual void _responder() = 0;

	public:
		serverObject( int domain, int service, int protocol, int port, u_long interface, int backlog );
		~serverObject();
		virtual void launch() = 0;
		listeningSocket* get_socket();

	};
}

#endif //WEBSERV_SERVEROBJECT_HPP
