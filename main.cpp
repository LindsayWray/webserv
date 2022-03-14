#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/time.h>
#include "sockets/listeningSocket.hpp"
#define SERVER_PORT 80

#define TRUE    1
#define FALSE   0

int main( void ) {
	webserv::listeningSocket server( AF_INET, SOCK_STREAM, 0, SERVER_PORT, INADDR_ANY, 32 );

	struct pollfd *fd_array = new(struct pollfd);
	memset( fd_array, 0, sizeof( fd_array ));
	fd_array[0].fd = server.get_sock();
	fd_array[0].events = POLLIN;
	int timeout = ( 3 * 60 * 1000 );
	int end_server = FALSE;
	int new_socket, ret;
	nfds_t n_fds = 1;
	while ( end_server == FALSE ) {
		ret = poll( fd_array, n_fds, timeout );
		if ( ret < 0 ) {
			std::cerr << "Poll() failed: " << strerror(errno) << std::endl;
			exit( 1 );
		}
		if ( ret == 0 ) {
			std::cerr << "Poll() timed out" << std::endl;
			exit( 1 );
		}

		if (( new_socket = accept( server.get_sock(), NULL, NULL )) < 0 ) {
			std::cerr << "Accept() failed: " << strerror(errno) << std::endl;
			end_server = TRUE;
		}

	}
}
