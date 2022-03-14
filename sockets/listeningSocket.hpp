//
// Created by Kester kas De rooij on 3/14/22.
//

#ifndef WEBSERV_LISTENINGSOCKET_HPP
#define WEBSERV_LISTENINGSOCKET_HPP

#include "serverSocket.hpp"

namespace webserv {

	class listeningSocket : public serverSocket {

	private:
		int _backlog;
		int _listening;

	public:
		listeningSocket( int domain, int service, int protocol, int port, u_long interface, int backlog );

		void start_listening( void );
	};
}

#endif //WEBSERV_LISTENINGSOCKET_HPP
