#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <string.h>
#include "server/testServer.hpp"
#include "utils/dataStructs.hpp"
#define SERVER_PORT 80

#define BUFFLEN 256

int main( void ) {
	webserv::socketData init;
	init.domain = AF_INET;
	init.service = SOCK_STREAM;
	init.protocol = 0;
	init.port = SERVER_PORT;
	init.interface = INADDR_ANY;
	webserv::listeningSocket server( init, 32 );
	webserv::readData received, sendd;
	struct pollfd incoming, outgoing;
	struct sockaddr_in address_buf;
	int ret;

	received.buflen = BUFFLEN;
	received.buf = new char[BUFFLEN];
	sendd.buflen = 6;
	sendd.buf = strdup("doei\n");
	incoming.fd = server.get_sock();
	incoming.events = POLLIN;
	while ( true ){
		ret = poll( &incoming, 1, 360000 );
		if ( ret < 0 ){
			std::cerr << " poll() failed " << strerror(errno) << std::endl;
			break;
		}
		if ( ret < 0 ){
			std::cout << " poll() timed out " << std::endl;
			break;
		}
		std::cout << " poll accepted " << ret << std::endl;
		address_buf = server.get_address();
		outgoing.fd = accept( server.get_sock(), (struct sockaddr *) &address_buf, (socklen_t *) &address_buf.sin_len);
		if ( outgoing.fd < 0 && errno != EWOULDBLOCK ){
			std::cerr << " accept() failed " << strerror(errno) << std::endl;
			break;
		}
		std::cout << " connection accepted " << outgoing.fd << std::endl;
		outgoing.events = POLLIN;
		received.bytesread = recv( outgoing.fd, received.buf, received.buflen, 0 );
		if ( received.bytesread < 0 && errno != EWOULDBLOCK ){
			std::cerr << " recv() failed " << strerror(errno) << std::endl;
			break;
		}
		if( received.bytesread == 0 ){
			std::cout << " no connection " << std::endl;
			break;
		}
		std::cout << " bytes read " << received.bytesread << std::endl;
		sendd.bytesread = send( outgoing.fd, sendd.buf, (size_t)sendd.buflen, 0 );
		if( sendd.bytesread < 0 ){
			std::cerr << " send() failed " << std::endl;
			break;
		}

	}

}
