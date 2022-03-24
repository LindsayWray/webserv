#include "utils/kqueueUtils.hpp"
#include "utils/dataStructs.hpp"
#include "server/server.hpp"
#include "http/HTTPResponseMessage.hpp"
#include <unistd.h>

int main( int argc, char **argv, char **env ) {
    if ( argc != 2 )
        return EXIT_FAILURE;
    SERVER_MAP serverMap;
    webserv::kqConData kqData;
    if ( init_servers(serverMap, argv[1], env, kqData) == ERROR )
        return EXIT_FAILURE;
    int nev, serv, current_fd;
    struct kevent* event = new struct kevent[MAX_EVENTS];
    kqData.nbr_connections = serverMap.size();
		std::cout << "NUMBER OF SERVER CONN: " << kqData.nbr_connections << std::endl;

	struct webserv::readData incoming;
	incoming.buflen = 1024;
	incoming.buf = new char[incoming.buflen];

	std::map<int,std::string> requests;

    /*
     * code is almost the same as the one in testServer.launch()
     * the logic behind the first bit should work **{prays to the gods}**
     * the accept function is now a non member function taking some parameters
     *
     * the while_ loop of the last else statement is practically unchanged from the one in testServer
     * so the request, handler and responder functions should be re-engineerd so that they are stand alone
     * you can access all the http data in serverMap
     *
     * serverMap[ fd ].first = listeningSocket;
     * serverMap[ fd ].second = httpData;
     *0
     */
    while ( true ){
        std::cout << "Waiting on kqueue() events..." << std::endl;
        nev = kevent(kqData.kq, NULL, 0, event, MAX_EVENTS, NULL);
        if ( nev == ERROR ) {
            std::cerr << "  kqueue() failed" << std::endl;
            break;
        } else if ( nev == 0 ){
            std::cerr << "  kqueue() timed out.  End program." << std::endl;
            break;
        }
        for ( int i = 0; i < nev; i++ ){
            current_fd = event[i].ident;
            if ( event[i].flags & EV_EOF ){		// check if it's an eof event, client disconnected
                std::cerr << "Disconnected" << std::endl;
                close( current_fd );
				kqData.nbr_connections--;
            } else if ( serverMap.find( current_fd ) != serverMap.end() )
                accept( serverMap[current_fd].first , kqData );
            else {
                while ( true ) {
                    memset( incoming.buf, 0, incoming.buflen);	//clean struct
                    incoming.bytesread = recv( current_fd, incoming.buf, incoming.buflen, 0 );
                    if ( incoming.bytesread < 0 ) {
                        if ( errno != EWOULDBLOCK )
                            std::cerr << "  recv() failed" << std::endl;
                        break;
                    }
                    if ( incoming.bytesread == 0 ) {
                        std::cout << "  Connection closed" << std::endl;
                        break;
                    }
                    requests[current_fd] += incoming.buf;
                    if ( requests[current_fd].find("\r\n\r\n") != std::string::npos ){
                        try{
                            webserv::Request request( requests[current_fd] );
                            std::cout << "made request object" << std::endl;
                            HTTPResponseMessage response = handler( request );
                            responder(current_fd, response);
                        }
                        catch( webserv::Request::IncorrectRequestException& e ){		// catches parsing errors from request
                            std::cout << e.what() << std::endl;
                        }
                        break;
                    }
                }
            }
        }
    }
	return EXIT_SUCCESS;
}

