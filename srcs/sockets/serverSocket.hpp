//
// Created by Kester kas De rooij on 3/14/22.
//

#ifndef WEBSERV_SERVERSOCKET_HPP
#define WEBSERV_SERVERSOCKET_HPP

#include "parentSocket.hpp"

namespace webserv {

	class serverSocket : public parentSocket {

	public:
		serverSocket( socketData input, int port );

		void connect_to_network( void );

		void set_nonblock( void );

		void set_sockt_opt( int options );
	};

}

#endif //WEBSERV_SERVERSOCKET_HPP
