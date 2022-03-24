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
		listeningSocket( socketData input, int port );
		void start_listening( void );
		bool is_listening( void );
	};
}

#endif //WEBSERV_LISTENINGSOCKET_HPP
