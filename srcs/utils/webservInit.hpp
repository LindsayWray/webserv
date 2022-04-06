#ifndef WEBSERV_WEBSERVINIT_HPP
#define WEBSERV_WEBSERVINIT_HPP


#include "../sockets/listeningSocket.hpp"
#include <unistd.h>

#define MAX_EVENTS 1024
#define SERVER_MAP std::map<int, std::pair<webserv::listeningSocket*,webserv::httpData*> >

namespace webserv {

    typedef struct serverData {
        webserv::kqConData kqData;
        SERVER_MAP serverMap;
        int CGI;
        int location_index;
        int current_fd;

        std::map<std::pair<int, std::string>, httpData *> host_servername;
        std::map<int, httpData *> default_server;
        std::map<int, std::string> requests;
        std::map<int, std::string> responses;
        std::map<int, httpData *> clientSockets;

        char *buf;
        int buflen;
    } serverData;

    int init_servers( webserv::serverData &serverData, std::string filename, char **env );

    std::string setFileLocation( char **env );
    int findPWD( char **env );

};

#endif
