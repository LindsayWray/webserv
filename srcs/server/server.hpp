#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/event.h>
#include <map>
#include "../sockets/listeningSocket.hpp"
#include "../http/Request.hpp"
#include "../http/HTTPResponseMessage.hpp"
#include "../utils/webservInit.hpp"
#include "../http/Request.hpp"

#define FINISHED true
#define NOT_FINISHED false

void accepter( std::pair<webserv::listeningSocket *, webserv::httpData *> &serverPair,
               webserv::kqConData &kqData, std::map<int, webserv::httpData *> &clientSockets );
HTTPResponseMessage handler( webserv::Request request, webserv::httpData *config, webserv::locationData location );
bool responder( int fd, std::map<int, std::string> &responses );
HTTPResponseMessage errorResponse( webserv::httpData *config, HTTPResponseMessage::e_responseStatusCode code);
void autoIndexing( std::string path, std::string directory, std::string &body );
HTTPResponseMessage::e_responseStatusCode CGI_register( webserv::locationData location, webserv::serverData &serverData, char **env, int client_fd, webserv::Request request);
int responseFromCGI( webserv::serverData &serverData, int pipe_fd );

class DirectoryNotFoundException : public std::exception {
public:
    const char *what() const throw() {
        return "Request Not Valid";
    }
};

#endif
