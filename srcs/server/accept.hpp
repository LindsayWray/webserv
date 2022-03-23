//
// Created by Kester kas De rooij on 3/23/22.
//

#ifndef WEBSERV_ACCEPT_HPP
#define WEBSERV_ACCEPT_HPP

#include "../utils/kqueueUtils.hpp"

void accept( webserv::listeningSocket* var, webserv::kqConData& kqData) {
    struct sockaddr_in address = var->get_address();
    int new_sd = 0;
    // std::cout << "trying to accept: SD " << new_sd << " nbconn " << _nb_of_conns << " Ncon " << _Ncon << std::endl;
    while ( new_sd != -1 && kqData.nbr_connections < kqData.worker_connections ) {
        new_sd = accept( var->get_sock(), (struct sockaddr *) &address, (socklen_t *) &address.sin_len);
        if ( new_sd < 0 ) {
            if (errno != EWOULDBLOCK) {
                std::cerr << errno << " " << strerror(errno) << std::endl;
                perror( "  accept() failed" ); // end server
            }
            return ;
        }
        printf( "  New incoming connection - %d\n", new_sd );
        struct kevent new_socket_change;
        EV_SET(&new_socket_change, new_sd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        kevent( kqData.kq, &new_socket_change, 1, NULL, 0, NULL); // listen for events on newly created socket
        kqData.nbr_connections++;
    }
}

#endif //WEBSERV_ACCEPT_HPP
