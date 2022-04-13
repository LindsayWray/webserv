#ifndef WEBSERV_CGI_HPP
#define WEBSERV_CGI_HPP

#include <unistd.h>
#include <sys/stat.h>
#include <sys/event.h>
#include "../http/HTTPResponseMessage.hpp"
#include "../utils/dataStructs.hpp"

namespace webserv {
// ******* CGIRegister.cpp *******  //
    HTTPResponseMessage::e_responseStatusCode CGIRegister( locationData location, serverData& serverData, int client_fd, Request request );

// ******* CGI_response.cpp *******  //
    int responseFromCGI( serverData& serverData, int pipe_fd );
}

#endif //WEBSERV_CGI_HPP
