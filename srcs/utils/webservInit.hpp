#ifndef WEBSERV_WEBSERVINIT_HPP
#define WEBSERV_WEBSERVINIT_HPP


#include "../sockets/listeningSocket.hpp"
#include <unistd.h>

#define MAX_EVENTS 1024
#define SERVER_MAP std::map<int, std::pair<webserv::listeningSocket*,webserv::httpData*> >

namespace webserv {

	typedef struct cgi_response {
		int client_fd;
		int pid;
		std::string body;
	} cgi_response;

    typedef struct serverData {
        webserv::kqConData kqData;
        SERVER_MAP serverMap;

		std::map<int, webserv::Request> requests;
        std::map<int, std::string> responses;
        std::map<int, webserv::httpData*> clientSockets;
		std::map<int, cgi_response> cgi_responses;

        char *buf;
        int buflen;
    } serverData;

    int init_servers( webserv::serverData &serverData, std::string filename, char **env );

    std::string setFileLocation( char **env );
    int findPWD( char **env );

};

#endif
