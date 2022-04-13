#ifndef WEBSERV_WEBSERV_HPP
#define WEBSERV_WEBSERV_HPP

#include <sys/event.h>
#include <map>
#include <unistd.h>
#include "../handler/handler.hpp"
#include "../sockets/listeningSocket.hpp"
#include "../CGI/CGI.hpp"
#include "../config/configParser.hpp"

#define MAX_EVENTS 1024
#define FINISHED true
#define NOT_FINISHED false

namespace webserv {
// ******* webserv.cpp *******  //
    void processEvent( serverData& serverData, struct kevent& event );

// ******* webservInit.cpp *******  //
    int init_servers( webserv::serverData& serverData, std::string filename, char** env );

// ******* accepter.cpp *******  //
    void accepter( std::pair<listeningSocket*, httpData>& serverPair, kqConData& kqData,
                   std::map<int, httpData>& clientSockets );

// ******* responder.cpp *******  //
    bool responder( int fd, std::map<int, std::string>& responses );
}

#endif //WEBSERV_WEBSERV_HPP
