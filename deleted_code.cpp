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
int main( void ){
	int end_server = FALSE, compress_array = FALSE;
	int nfds = 1, current_size;
	int new_sd, close_conn;
	char buffer[80];
	while ( end_server == FALSE ) {
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
		for ( int i = 0; i < current_size; i++ ) {
			if ( fds[i].revents == 0 )
				continue;
			if ( fds[i].revents != POLLIN ) {
				printf( "  Error! revents = %d\n", fds[i].revents );
				end_server = TRUE;
				break;
			}
			if ( fds[i]._socket == socket_fd ) {
				printf( "  Listening sockOb is readable\n" );
				while ( new_sd != -1 ) {
					new_sd = accept( socket_fd, NULL, NULL );
					if ( new_sd < 0 ) {
						if ( errno != EWOULDBLOCK ) {
							perror( "  accept() failed" );
							end_server = TRUE;
						}
						break;
					}
					/*
					 * int accept(int sockfd, struct sockaddr *restrict address, socklen_t *restrict address_len)
					 *
					 * sockfd = socket_fd
					 * address = NULL (could also be sockaddr structure for the connecting sockOb adress)
					 * address_len = pointer to socklen_t for the len of supploed sockaddr
					 *
					 * EWOULDBLOCK in case O_NONBLOCK is set, any other failure will end server
					 */
					printf( "  New incoming connection - %d\n", new_sd );
					fds[nfds]._socket = new_sd;
					fds[nfds].events = POLLIN;
					nfds++;
				}
			}
			else {
				printf( "  Descriptor %d is readable\n", fds[i]._socket );
				close_conn = FALSE;
				while ( TRUE ) {
					ret = recv( fds[i]._socket, buffer, sizeof( buffer ), 0 );
					if ( ret < 0 ) {
						if ( errno != EWOULDBLOCK ) {
							perror( "  recv() failed" );
							close_conn = TRUE;
						}
						break;
					}
					/*
					 * ssize_t recv(int sockfd, void *buf, size_t len, int flags)
					 *
					 * sockfd = socket_fd
					 * buf = destination for the data read
					 * len = size of buffer
					 * flags = 0
					 * ret = is size read
					 *
					 * EWOULDBLOCK in case O_NONBLOCK is set
					 */
					if ( ret == 0 ) {
						printf( "  Connection closed\n" );
						close_conn = TRUE;
						break;
					}
					printf( "  %d bytes received\n", ret );
					ret = send( fds[i]._socket, buffer, ret, 0 );
					if ( ret < 0 ) {
						perror( "  send() failed" );
						close_conn = TRUE;
						break;
					}
					/*
					 * ssize_t send(int sockfd, const void *buf, size_t len, int flags)
					 *
					 * sockfd = socket_fd
					 * buf = data to transmit
					 * len = len of buf
					 * flags = 0
					 * ret = bytes send (-1 is error)
					 */
				}
				if ( close_conn ) {
					close( fds[i]._socket );
					fds[i]._socket = -1;
					compress_array = TRUE;
				}
			}
		}
		/***********************************************************/
		/* If the compress_array flag was turned on, we need       */
		/* to squeeze together the array and decrement the number  */
		/* of file descriptors. We do not need to move back the    */
		/* events and revents fields because the events will always*/
		/* be POLLIN in this case, and revents is output.          */
		/***********************************************************/
		if ( compress_array ) {
			compress_array = FALSE;
			for ( int i = 0; i < nfds; i++ ) {
				if ( fds[i]._socket == -1 ) {
					for ( int j = i; j < nfds; j++ ) {
						fds[j]._socket = fds[j + 1]._socket;
					}
					i--;
					nfds--;
				}
			}
		}
	}
	for ( int i = 0; i < nfds; i++ ) {
		if ( fds[i]._socket >= 0 )
			close( fds[i]._socket );
	}
}