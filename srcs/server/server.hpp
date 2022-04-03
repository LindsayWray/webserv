#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/event.h>
#include <map>
#include "../sockets/listeningSocket.hpp"
#include "../http/Request.hpp"
#include "../http/HTTPResponseMessage.hpp"

#define FINISHED true
#define NOT_FINISHED false

	void 				accepter( std::pair<webserv::listeningSocket*,webserv::httpData*>& serverPair, 
								webserv::kqConData& kqData,std::map<int,webserv::httpData*>& clientSockets);
	HTTPResponseMessage handler( webserv::Request request, webserv::httpData* config );
	bool				responder(int fd, std::map<int,std::string>& responses);
	void				fileNotFound(HTTPResponseMessage& response, webserv::httpData* config );
	void				autoIndexing( std::string path, std::string directory, std::string& body );
	HTTPResponseMessage	responseFromCGI(webserv::httpData* config, webserv::locationData *location );

	class DirectoryNotFoundException : public std::exception{
		public:
			const char* what() const throw(){
				return "Request Not Valid";
			}
		};

#endif
