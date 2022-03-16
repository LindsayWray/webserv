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
	webserv::testServer server( init, 32, 100 );
	server.launch();

}
