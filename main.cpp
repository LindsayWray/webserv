#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>

#define SERVER_PORT 80

#define TRUE    1
#define FALSE   0

int main( void ) {
	int server_fd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( server_fd < 0 ) {
		std::cerr << "Socket failed: " << strerror(errno) << std::endl;
		exit( EXIT_FAILURE );
	}
	/*
	 * int socket(int domain, int type, int protocol)
	 *
	 * domain = AF_INET6 (IPv6)
	 * type(of socket) = SOCK_STREAM
	 *  -   TCP
	 *  -   sequenced, reliable, 2 way connection based byte stream
	 *  -   full-duplex (similar to pipes)
	 * protocol = 0
	 */

	int optval = 1;
	if ( setsockopt( server_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof( optval )) < 0 ) {
		std::cerr << "Setsockopt() failed: " << strerror(errno) << std::endl;
		close( server_fd );
		exit( EXIT_FAILURE );
	}
	/*
	 * int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len)
	 *
	 * socket = integer resulting from socket()
	 * level = SOL_SOCKET -> manipulate options at socket level
	 * option_name =
	 * -    SO_REUSEADDR = allow socket descriptor to be reusable <sys/socket.h>
	 * -    SO_REUSEPORT = allow port to be reusable
	 * "The parameters option_value and option_len are used to access option values for setsockopt()"
	 */

	if ( fcntl( server_fd, O_NONBLOCK ) < 0 ) {
		std::cerr << "Fcntl() failed: " << strerror(errno) << std::endl;
		close( server_fd );
		exit( EXIT_FAILURE );
	}
	/*
	 * int(int fildes, int cmd, ...)
	 *
	 * fildes = our socket
	 * cmd = set to non-blocking
	 */

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( SERVER_PORT );
	/*
	 * uint16_t htons(uint16_t hostshort)
	 *
	 * hostshort = port to connect
	 *
	 * htons = for port
	 * htonl = for address
	 */

	if ( bind( server_fd, (struct sockaddr *) &address, sizeof( address )) < 0 ) {
		std::cerr << "Bind() failed: " << strerror(errno) << std::endl;
		close( server_fd );
		exit( EXIT_FAILURE );
	}
	/*
	 * int bind(int socket, const struct sockaddr *address, socklen_t address_len)
	 *
	 * assign address to socket
	 */

	if ( listen( server_fd, 32 )) {
		std::cerr << "Listen() failed: " << strerror(errno) << std::endl;
		close( server_fd );
		exit( EXIT_FAILURE );
	}
	/*
	 * int listen(int socket, int backlog)
	 *
	 * listen for connection on socket
	 * backlog defines the maximum length for the queue of pending connections
	 */

	struct pollfd *fd_array = new(struct pollfd);
	memset( fd_array, 0, sizeof( fd_array ));
	fd_array[0].fd = server_fd;
	fd_array[0].events = POLLIN;
	int timeout = ( 3 * 60 * 1000 );
	/*
	 * .fd = our socket fd
	 * .events = POLLIN (incoming signals in poll)
	 */
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

		if (( new_socket = accept( server_fd, NULL, NULL )) < 0 ) {
			std::cerr << "Accept() failed: " << strerror(errno) << std::endl;
			end_server = TRUE;
		}

	}
}
