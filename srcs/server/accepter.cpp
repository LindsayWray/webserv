#include "server.hpp"
#include "../utils/webserv.hpp"

void accepter( std::pair<webserv::listeningSocket*,webserv::httpData*>& serverPair,
               webserv::kqConData &kqData,std::map<int,webserv::httpData*>& clientSockets) {
    struct sockaddr_in address = serverPair.first->get_address();
    int new_sd = 0;
    if ( kqData.nbr_connections < kqData.worker_connections ) {
        new_sd = accept( serverPair.first->get_sock(), (struct sockaddr *)&address, (socklen_t *)&address.sin_len);
        if ( new_sd < 0 ) {
            if (errno != EWOULDBLOCK) {
                std::cerr << errno << " " << strerror(errno) << std::endl;
                perror( "  accept() failed" ); // end server
            }
            return;
        }
        printf( "  New incoming connection - %d\n", new_sd );
        struct kevent new_socket_change;
        EV_SET( &new_socket_change, new_sd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL );
        if ( kevent( kqData.kq, &new_socket_change, 1, NULL, 0, NULL ) == ERROR ){
            return webserv::kqueueFailure( new_sd );
        }
		clientSockets[new_sd] = serverPair.second;
        kqData.nbr_connections++;
    }
}

