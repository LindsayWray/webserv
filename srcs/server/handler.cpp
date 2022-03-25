#include "server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

using webserv::Request;

namespace fs = std::__fs::filesystem;

std::string file_extension(std::string path) {
	int pos = path.find('.');
	if (pos == std::string::npos) 
		return "";
	printf("file extension: %s\n", path.substr(pos + 1).c_str());
	return path.substr(pos + 1); // +1 to skip the .
}

void GET_handler( Request request, HTTPResponseMessage& response, std::string& root ) {
	std::ifstream file;
	std::ifstream defaultFile;
	std::string line;
	std::string body("");

	std::string path = root + request.getPath();
	std::string extension = file_extension(request.getPath());
	std::cout << "EXTENSION: "  << extension << "end" << std::endl;

	// if(!fs::exists(path)) {
	// 	std::cout << "File not found " << path << std::endl;
	// 	body = "Not Found";
	// 	response.addStatus(HTTPResponseMessage::NOT_FOUND)
	// 				.addTypeExt("plain");
	// }
	// else if (fs::is_directory(path)) {
	// 	std::cout << "Is a directory " << path << std::endl;
	// 	defaultFile.open(root + "default.html");
	// 	while( std::getline( file, line ) )
	// 		body += (line + '\n');
	// 	response.addStatus(HTTPResponseMessage::OK)
	// 						.addTypeExt("html");
	// }
	// else if (fs::is_regular_file(path)) {
	// 	file.open(path);
	// 	if(!file.good()){
	// 		std::cout << "No Access to file " << path << std::endl;
	// 		body = "No Access";
	// 		response.addStatus(HTTPResponseMessage::FORBIDDEN)
	// 				.addTypeExt("plain");
	// 	}
	// 	else {
	// 		std::cout << "File found " << path << std::endl;
	// 		while( std::getline( file, line ) )
	// 			body += (line + '\n');
	// 		response.addStatus(HTTPResponseMessage::OK)
	// 			.addTypeExt("html");
	// 	}
	// }
	// response.addLength(body.length());
	// response.addBody(body);

	file.open(path);
	if (file.good()) {
		std::cout << "File found " << path << std::endl;
		while( std::getline( file, line ) ) {
			body += (line + '\n');
		}
		response.addStatus(HTTPResponseMessage::OK)
							.addLength(body.length())
							.addBody(body)
							.addType(HTTPResponseMessage::contentTypes.at(extension));
	} else {
		std::cout << "File not found " << path << std::endl;
		body = "Not Found";
		response.addStatus(HTTPResponseMessage::NOT_FOUND)
					.addLength(body.length())
					.addBody(body)
					.addType(HTTPResponseMessage::contentTypes.at(extension));
	}
}


void POST_handler( Request request, HTTPResponseMessage& response, std::string& root ) {}


void DELETE_handler( Request request, HTTPResponseMessage& response, std::string& root ) {}

HTTPResponseMessage handler( Request request ) {
	HTTPResponseMessage response;
	std::string root;

	//root = this->_http.locations[0].root;

	if ( request.getMethod() == Request::GET )
		GET_handler( request, response, root );
	else if ( request.getMethod() == Request::POST )
		POST_handler( request, response, root );
	else
		DELETE_handler( request, response, root);
	return response;
}