#include "utils/kqueueUtils.hpp"
#include "../server/accept.hpp"
#include <unistd.h>

int main( int argc, char **argv, char **env ) {
    if ( argc != 2 )
        return EXIT_FAILURE;
    SERVER_MAP serverMap;
    webserv::kqConData kqData;
    if ( init_servers(serverMap, argv[1], env, kqData) == ERROR )
        return EXIT_FAILURE;
    int nev, serv, current_fd;
    struct kevent* event = new struct kevent[kqData.worker_connections];
    kqData.nbr_connections = 1;

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
     *
     */
    while ( true ){
        std::cout << "Waiting on kqueue() events..." << std::endl;
        nev = kevent(kqData.kq, NULL, 0, event, kqData.worker_connections, NULL);
        if ( nev == ERROR ) {
            std::cerr << "  kqueue() failed" << std::endl;
            break;
        } else if ( nev == 0 ){
            std::cerr << "  kqueue() timed out.  End program." << std::endl;
            break;
        }
        for ( int i = 0; i < nev; i++ ){
            current_fd = event[i].ident;
            if ( event[i].flags & EV_EOF ){
                std::cerr << "Disconnected" << std::endl;
                close( current_fd );
            } else if ( serverMap.find( current_fd ) != serverMap.end() )
                accept( serverMap.find( current_fd )->second.first , kqData );
            else {
                while ( true ) {
                    memset( _incoming.buf, 0, _incoming.buflen);	//clean struct
                    _incoming.bytesread = recv( current_fd, _incoming.buf, _incoming.buflen, 0 );
                    if ( _incoming.bytesread < 0 ) {
                        if ( errno != EWOULDBLOCK )
                            std::cerr << "  recv() failed" << std::endl;
                        break;
                    }
                    if ( _incoming.bytesread == 0 ) {
                        std::cout << "  Connection closed" << std::endl;
                        break;
                    }
                    _requests[current_fd] += _incoming.buf;
                    if ( _requests[current_fd].find("\r\n\r\n") != std::string::npos ){
                        try{
                            webserv::Request request( _requests[current_fd] );
                            std::cout << "made request object" << std::endl;
                            HTTPResponseMessage response = _handler( request);
                            _responder(current_fd, response);
                        }
                        catch( Request::IncorrectRequestException& e ){		// catches parsing errors from request
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

