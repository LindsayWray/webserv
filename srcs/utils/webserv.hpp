#ifndef WEBSERV_WEBSERV_HPP
#define WEBSERV_WEBSERV_HPP


#include "../sockets/listeningSocket.hpp"
#include <unistd.h>

#define SERVER_MAP std::map<int, std::pair<webserv::listeningSocket*,webserv::httpData*> >

namespace webserv {

    typedef struct serverData {
        webserv::kqConData kqData;
        SERVER_MAP serverMap;

        // int CGI;
        // int location_index;
        // int current_fd;

		std::map<int, std::pair<int, std::string> > cgi_repsonses; // key: cgi_pipe, value: client_socket + cgi_response
        std::map<int, std::string> requests;
        std::map<int, std::string> responses;
        std::map<int, webserv::httpData *> clientSockets;

        char *buf;
        int buflen;
    } serverData;
};

#endif
