#include "webserv/webserv.hpp"

using namespace webserv;

int main( int argc, char** argv, char** env ) {

    if ( argc > 2 ) {
        std::cout << "Incorrect program usage\n	RUN: ./webserv [configuration file]\n";
        return EXIT_FAILURE;
    }
    std::string config = argc == 1 ? "config.webserv" : argv[1];
    webserv::serverData serverData;

    if ( init_servers( serverData, config, env ) == ERROR )
        return EXIT_FAILURE;

    struct kevent* eventList = new struct kevent[MAX_EVENTS];
    while ( true ) {
        std::cout << "Waiting on kqueue() events..." << std::endl;
        int nev = kevent( serverData.kqData.kq, NULL, 0, eventList, MAX_EVENTS, NULL );
        if ( nev == ERROR ) {
            std::cerr << "  kqueue() failed" << std::endl;
            break;
        } else if ( nev == 0 ) {
            std::cerr << "  kqueue() timed out.  End program." << std::endl;
            break;
        }
        for ( int i = 0; i < nev; i++ ) {
            processEvent( serverData, eventList[i] );
        }
    }
    return EXIT_SUCCESS;
}
