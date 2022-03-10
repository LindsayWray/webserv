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
	int socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( socket_fd < 0 ) {
		perror( "socket() failed" );
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
	if ( setsockopt( socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof( optval )) < 0 ) {
		perror( "setsockopt() failed" );
		close( socket_fd );
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

	if ( fcntl( socket_fd, O_NONBLOCK ) < 0 ) {
		perror( "fcntl() failed" );
		close( socket_fd );
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
	 * uint16_t (uint16_t hostshort)
	 *
	 * hostshort = port to connect
	 */

	if ( bind( socket_fd, (struct sockaddr *) &address, sizeof( address )) < 0 ) {
		perror( "bind() failed" );
		close( socket_fd );
		exit( EXIT_FAILURE );
	}
	/*
	 * int bind(int socket, const struct sockaddr *address, socklen_t address_len)
	 *
	 * assign address to socket
	 */
	if ( listen( socket_fd, 32 )) {
		perror( "listen() failed" );
		close( socket_fd );
		exit( EXIT_FAILURE );
	}
	/*
	 * int listen(int socket, int backlog)
	 *
	 * listen for connection on socket
	 * backlog defines the maximum length for the queue of pending connections
	 */
	struct pollfd fds[200];
	memset( fds, 0, sizeof( fds ));
	fds[0].fd = socket_fd;
	fds[0].events = POLLIN;
	int timeout = ( 3 * 60 * 1000 );
	/*
	 * .fd = our socket fd
	 * .events = POLLIN (incoming signals in poll)
	 */
	int end_server = FALSE, compress_array = FALSE;
	int nfds = 1, current_size = 0, i, j;
	int new_sd, close_conn, len;
	char buffer[80];
	do {
		std::cout << "Waiting on poll()...\n" << std::endl;
		int ret = poll( fds, nfds, timeout );
		if ( ret < 0 ) {
			perror( " poll() failed" );
			break;
		}
		if ( ret == 0 ) {
			printf( "  poll() timed out.  End program.\n" );
			break;
		}
		current_size = nfds;
		for ( i = 0; i < current_size; i++ ) {
			if ( fds[i].revents == 0 )
				continue;
			if ( fds[i].revents != POLLIN ) {
				printf( "  Error! revents = %d\n", fds[i].revents );
				end_server = TRUE;
				break;
			}
			if ( fds[i].fd == socket_fd ) {
				printf( "  Listening socket is readable\n" );
				/*******************************************************/
				/* Accept all incoming connections that are            */
				/* queued up on the listening socket before we         */
				/* loop back and call poll again.                      */
				/*******************************************************/
				do {
					/*****************************************************/
					/* Accept each incoming connection. If               */
					/* accept fails with EWOULDBLOCK, then we            */
					/* have accepted all of them. Any other              */
					/* failure on accept will cause us to end the        */
					/* server.                                           */
					/*****************************************************/
					new_sd = accept( socket_fd, NULL, NULL );
					if ( new_sd < 0 ) {
						if ( errno != EWOULDBLOCK ) {
							perror( "  accept() failed" );
							end_server = TRUE;
						}
						break;
					}

					/*****************************************************/
					/* Add the new incoming connection to the            */
					/* pollfd structure                                  */
					/*****************************************************/
					printf( "  New incoming connection - %d\n", new_sd );
					fds[nfds].fd = new_sd;
					fds[nfds].events = POLLIN;
					nfds++;

					/*****************************************************/
					/* Loop back up and accept another incoming          */
					/* connection                                        */
					/*****************************************************/
				} while ( new_sd != -1 );
			}

				/*********************************************************/
				/* This is not the listening socket, therefore an        */
				/* existing connection must be readable                  */
				/*********************************************************/

			else {
				printf( "  Descriptor %d is readable\n", fds[i].fd );
				close_conn = FALSE;
				/*******************************************************/
				/* Receive all incoming data on this socket            */
				/* before we loop back and call poll again.            */
				/*******************************************************/

				do {
					/*****************************************************/
					/* Receive data on this connection until the         */
					/* recv fails with EWOULDBLOCK. If any other         */
					/* failure occurs, we will close the                 */
					/* connection.                                       */
					/*****************************************************/
					ret = recv( fds[i].fd, buffer, sizeof( buffer ), 0 );
					if ( ret < 0 ) {
						if ( errno != EWOULDBLOCK ) {
							perror( "  recv() failed" );
							close_conn = TRUE;
						}
						break;
					}

					/*****************************************************/
					/* Check to see if the connection has been           */
					/* closed by the client                              */
					/*****************************************************/
					if ( ret == 0 ) {
						printf( "  Connection closed\n" );
						close_conn = TRUE;
						break;
					}

					/*****************************************************/
					/* Data was received                                 */
					/*****************************************************/
					len = ret;
					printf( "  %d bytes received\n", len );

					/*****************************************************/
					/* Echo the data back to the client                  */
					/*****************************************************/
					ret = send( fds[i].fd, buffer, len, 0 );
					if ( ret < 0 ) {
						perror( "  send() failed" );
						close_conn = TRUE;
						break;
					}

				} while ( TRUE );

				/*******************************************************/
				/* If the close_conn flag was turned on, we need       */
				/* to clean up this active connection. This            */
				/* clean up process includes removing the              */
				/* descriptor.                                         */
				/*******************************************************/
				if ( close_conn ) {
					close( fds[i].fd );
					fds[i].fd = -1;
					compress_array = TRUE;
				}


			}  /* End of existing connection is readable             */
		} /* End of loop through pollable descriptors              */

		/***********************************************************/
		/* If the compress_array flag was turned on, we need       */
		/* to squeeze together the array and decrement the number  */
		/* of file descriptors. We do not need to move back the    */
		/* events and revents fields because the events will always*/
		/* be POLLIN in this case, and revents is output.          */
		/***********************************************************/
		if ( compress_array ) {
			compress_array = FALSE;
			for ( i = 0; i < nfds; i++ ) {
				if ( fds[i].fd == -1 ) {
					for ( j = i; j < nfds; j++ ) {
						fds[j].fd = fds[j + 1].fd;
					}
					i--;
					nfds--;
				}
			}
		}

	} while ( end_server == FALSE );

	for ( i = 0; i < nfds; i++ ) {
		if ( fds[i].fd >= 0 )
			close( fds[i].fd );
	}
}

