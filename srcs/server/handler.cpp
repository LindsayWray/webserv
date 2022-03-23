#include "testServer.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>

using webserv::Request;

std::string file_extension(std::string path) {
	int pos = path.find('.');
	if (pos == std::string::npos) 
		return "";
	printf("file extension: %s\n", path.substr(pos + 1).c_str());
	return path.substr(pos + 1); // +1 to skip the .
}



void GET_handler( Request request, HTTPResponseMessage& response, std::string& root ) {
	std::ifstream file;
	std::string line;
	std::string body("");

	std::string path = root + request.getPath();

	std::string extension = file_extension(request.getPath());
	// std::cout << extension << std::endl;

	file.open(path);
	// file.seekg(0, std::ios::end); // This will set the fail bit if fileName is a directory (or do nothing if it is already set  


	if (file.good()) {
		std::cout << "File found " << path << std::endl;
		while( std::getline( file, line ) ) {
			body += (line + '\n');
		}
		response.addStatus(HTTPResponseMessage::OK)
							//.addTypeExt("text/html") // tells the client which datatype it can expect in the body
							.addLength(body.length())
							.addBody(&body);
	} else {
		std::cout << "File not found " << path << std::endl;

		body = "Not Found";
		response.addStatus(HTTPResponseMessage::NOT_FOUND)
					//.addTypeExt("text/plain")
					.addLength(body.length())
					.addBody(&body);
	}
}


void POST_handler( Request request, HTTPResponseMessage& response, std::string& root ) {}


void DELETE_handler( Request request, HTTPResponseMessage& response, std::string& root ) {}

HTTPResponseMessage webserv::testServer::_handler( Request request ) {
	HTTPResponseMessage response;
	std::string root;

	root = this->_http.locations[0].root;

	if ( request.getMethod() == Request::GET )
		GET_handler( request, response, root );
	else if ( request.getMethod() == Request::POST )
		POST_handler( request, response, root );
	else
		DELETE_handler( request, response, root);
	return response;
}