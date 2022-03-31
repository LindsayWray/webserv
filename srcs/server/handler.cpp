#include "server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>
#include <string> 
#include <dirent.h>


using webserv::Request;

static std::string file_extension(std::string path) {
	int pos = path.find('.');
	if (pos == std::string::npos) 
		return "";
	return path.substr(pos + 1); // +1 to skip the .
}

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

static void fileNotFound(HTTPResponseMessage& response, webserv::httpData* config ){
	std::string body = "Not Found";
	std::string path;
	std::ifstream file;

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
	response.addStatus(HTTPResponseMessage::NOT_FOUND)
				.addLength(body.length())
				.addBody(body)
				.addType("text/html");
};

void GET_handler( Request request, HTTPResponseMessage& response, std::string path, webserv::httpData* config ) {
	std::ifstream file;

	std::string extension = file_extension(path);
	std::cout << "EXTENSION: "  << extension << std::endl;

	if (path.back() == '/') {
		std::cout << "Is a directory " << path << std::endl;

		// insert autoindexing here?...

		return GET_handler(request, response, path + "index.html", config);
	}

	if (path.find("AUTOINDEX.HTML") == path.size() - strlen("AUTOINDEX.HTML")) {
		std::string directory = path.substr(0, path.find("AUTOINDEX.HTML"));

		std::cout << "going autoindexing " << directory << std::endl;

		std::string body;

		DIR *dir;
		struct dirent *entry;
		if ((dir = opendir(directory.c_str())) != NULL) { //print all the files and directories within directory
			readdir(dir);	// skip the first folder
			while ((entry = readdir(dir)) != NULL) {
				body += "<a href=\"" + request.getPath() + entry->d_name + "\">";
				body += entry->d_name;
				if (entry->d_type == DT_DIR)
					body += "/\n";
				body += "</a><br>";
			}
			closedir (dir);
		} else //could not open directory
			return fileNotFound(response, config);
		response.addStatus(HTTPResponseMessage::OK)
			.addBody(body)
			.addLength(body.length())
			.addType("text/html");
		return;
	}

	file.open(path);
	if (file.good()) {
		std::cout << "File found " << path << std::endl;
		responseFromFile(file, extension, response, HTTPResponseMessage::OK);
	} else
		fileNotFound(response, config);
}


void POST_handler( Request request, HTTPResponseMessage& response, std::string& path, webserv::httpData* config ) {
	std::string extension = file_extension(path);
	if (extension != "txt")
		response.addStatus(HTTPResponseMessage::METHOD_NOT_ALLOWED);
	else if (bool fileAlreadyExists = false)	// add fileExists check
		response.addStatus(HTTPResponseMessage::METHOD_NOT_ALLOWED);
	else {
		response.addStatus(HTTPResponseMessage::ACCEPTED);
		std::ofstream file;
		file.open(path, std::ios::out);
		if (file.good()) {
			file << request.getBody();
			config->created_files[request.getPath()] = path;
		}
		file.close();
	}
}

void DELETE_handler( Request request, HTTPResponseMessage& response, std::string& path, webserv::httpData* config ) {
	std::map<std::string, std::string>::iterator fileToBeDeleted = config->created_files.find(request.getPath());
	if (fileToBeDeleted != config->created_files.end()) {
		if (std::remove(fileToBeDeleted->second.c_str()) == 0)
			config->created_files.erase(fileToBeDeleted);
		else
			response.addStatus(HTTPResponseMessage::INTERNAL_SERVER_ERROR);
	} else {
		response.addStatus(HTTPResponseMessage::INTERNAL_SERVER_ERROR);
	}
}

HTTPResponseMessage handler( Request request, webserv::httpData* config ) {
	HTTPResponseMessage response;
	std::string fullPath;

	fullPath = config->getRequestedFilePath(request.getPath());

	std::cout << "PATH: " << fullPath << std::endl;

	if ( request.getMethod() == Request::GET )
		GET_handler( request, response, fullPath, config);
	else if ( request.getMethod() == Request::POST )
		POST_handler( request, response, fullPath, config);
	else
		DELETE_handler( request, response, fullPath, config);
	return response;
}