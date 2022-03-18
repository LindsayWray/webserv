#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <string.h>
#include "server/testServer.hpp"
#include "utils/dataStructs.hpp"
#define SERVER_PORT 80

#define BUFFLEN 256

int main( int argc, char **argv, char **envp ) {

	webserv::socketData init;
	init.addPort( SERVER_PORT );
	init.backlog = 32;
	init.worker_connections = 100;
	webserv::testServer server( init );
//    for (int i = 0; envp[i]; i++){
//        for (int j = 0; envp[i][j]; j++)
//            std::cout << envp[i][j];
//        std::cout << std::endl;
//    }
	server.launch();
}
