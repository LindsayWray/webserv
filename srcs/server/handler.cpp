#include "server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <string> 


#include <experimental/filesystem> // C++14
namespace fs = std::experimental::filesystem;

using webserv::Request;

// namespace fs = std::__fs::filesystem;

static std::string file_extension(std::string path) {
	int pos = path.find('.');
	if (pos == std::string::npos) 
		return "";
	return path.substr(pos + 1); // +1 to skip the .
}

static void fileNotFound(HTTPResponseMessage& response){
	std::string body = "Not Found";
	response.addStatus(HTTPResponseMessage::NOT_FOUND)
				.addLength(body.length())
				.addBody(body)
				.addType("text/html");
};

static void responseFromFile(std::ifstream& file, std::string extension, HTTPResponseMessage& response, HTTPResponseMessage::e_responseStatusCode statusCode) {
	std::string line;
	std::string body("");

	while( std::getline( file, line ) ) {
		body += (line + '\n');
	}
	file.close();
	response.addStatus(statusCode)
			.addLength(body.length())
			.addBody(body);
	try {
		response.addType(HTTPResponseMessage::contentTypes.at(extension));
	}
	catch (...) {
		response.addType("text/plain");   //temporary fix until directory handling 
	}
}

void GET_handler( Request request, HTTPResponseMessage& response, std::string path, webserv::httpData* config ) {
	std::ifstream file;

	std::string extension = file_extension(path);
	std::cout << "EXTENSION: "  << extension << std::endl;

	//if (fs::is_directory(path)) {
	if (*path.rbegin() == '/') {
		std::cout << "Is a directory " << path << std::endl;

		// insert autoindexing here?...

		return GET_handler(request, response, path + "index.html", config);
	}

	file.open(path);
	if (file.good()) {
		std::cout << "File found " << path << std::endl;
		responseFromFile(file, extension, response, HTTPResponseMessage::OK);
	} else { //404
		if (config->error_page.find(404) != config->error_page.end()){
			path = config->getRequestedFilePath(config->error_page[404]);
			std::cout << "Get error page " << path << std::endl;
			file.open(path);
			if (file.good()) {
				std::cout << "Error file found " << path << std::endl;
				return responseFromFile(file, file_extension(path), response, HTTPResponseMessage::NOT_FOUND);
			}
		}
		std::cout << "FILE not found " << path << std::endl;
		fileNotFound(response);
	}
}


void POST_handler( Request request, HTTPResponseMessage& response, std::string& root ) {
	response.addStatus(HTTPResponseMessage::NOT_IMPLEMENTED);
}


void DELETE_handler( Request request, HTTPResponseMessage& response, std::string& root ) {
	response.addStatus(HTTPResponseMessage::NOT_IMPLEMENTED);
}

HTTPResponseMessage handler( Request request, webserv::httpData* config ) {
	HTTPResponseMessage response;
	std::string fullPath;

	fullPath = config->getRequestedFilePath(request.getPath());

	std::cout << "PATH: " << fullPath << std::endl;

	if ( request.getMethod() == Request::GET )
		GET_handler( request, response, fullPath, config);
	else if ( request.getMethod() == Request::POST )
		POST_handler( request, response, fullPath );
	else
		DELETE_handler( request, response, fullPath);
	return response;
}