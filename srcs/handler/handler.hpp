#ifndef WEBSERV_HANDLER_HPP
#define WEBSERV_HANDLER_HPP

#include <sys/event.h>
#include <map>
#include "../utils/dataStructs.hpp"
#include "../http/Request.hpp"
#include "../http/HTTPResponseMessage.hpp"

namespace webserv {
// ******* handler.cpp *******  //
    HTTPResponseMessage handler( Request request, httpData config, locationData location );

// ******* autoindexing.cpp *******  //
    void autoIndexing( std::string path, std::string directory, std::string& body );

// ******* handlerUtils.cpp *******  //
    HTTPResponseMessage errorResponse( httpData config, HTTPResponseMessage::e_responseStatusCode code );

    HTTPResponseMessage responseWhenFileAlreadyExists( httpData server );

    HTTPResponseMessage responseWhenFileCreated( std::string requestURL );

    HTTPResponseMessage responseWhenFileDeleted( std::string requestURL );

    HTTPResponseMessage responseWhenFileCantBeDeleted( std::string requestURL );

// ******* exception *******  //
    class DirectoryNotFoundException : public std::exception {
    public:
        const char* what() const throw() {
            return "Request Not Valid";
        }
    };
}
#endif //WEBSERV_HANDLER_HPP
