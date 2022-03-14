//
// Created by Kester kas De rooij on 3/14/22.
//

#ifndef WEBSERV_CLIENTSOCKET_HPP
#define WEBSERV_CLIENTSOCKET_HPP

#include "parentSocket.hpp"

namespace webserv {

	class clientSocket : public parentSocket {

	public:
		clientSocket( int domain, int service, int protocol, int port, u_long interface );

		void connect_to_network( void );
	};

}

#endif //WEBSERV_CLIENTSOCKET_HPP
