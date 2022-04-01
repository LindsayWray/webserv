#include "server.hpp"
#include <iostream>
#include <string>

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

	while( std::getline( file, line ) )
		body += (line + '\n');
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

void fileNotFound(HTTPResponseMessage& response, webserv::httpData* config ){
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

void GET_handler( std::string path, HTTPResponseMessage& response, webserv::httpData* config, webserv::locationData* location ) {
	std::ifstream file;
	std::string extension = file_extension(path);
	std::cout << "EXTENSION: "  << extension << std::endl;
	std::string fullPath = location->root + path;

	if (path.back() == '/') {
		std::cout << "Is a directory " << path << std::endl;
		if (location->autoindex) {
			std::string body;
			try {
				autoIndexing(path, fullPath, body);
			}
			catch ( DirectoryNotFoundException& e){
				std::cout << e.what() << std::endl;
				return fileNotFound(response, config);
			}	
			response.addStatus(HTTPResponseMessage::OK)
				.addBody(body)
				.addLength(body.length())
				.addType("text/html");
			return;
		}
		return GET_handler(path + "index.html", response, config, location);
	}

	//webserv::locationData *location = config->_findLocationBlock(request.getPath());
	if ( location && location->CGI )
		return responseFromCGI( config, location, response );

	file.open(fullPath);
	if (file.good()) {
		std::cout << "File found " << fullPath << std::endl;
		responseFromFile(file, extension, response, HTTPResponseMessage::OK);
	} else
		fileNotFound(response, config);
}

// void POST_handler( Request request, HTTPResponseMessage& response, webserv::httpData* config, webserv::locationData location ) {
// 	std::string extension = file_extension(path);
// 	if (extension != "txt")
// 		response.addStatus(HTTPResponseMessage::METHOD_NOT_ALLOWED);
// 	else if (bool fileAlreadyExists = false)	// add fileExists check
// 		response.addStatus(HTTPResponseMessage::METHOD_NOT_ALLOWED);
// 	else {
// 		response.addStatus(HTTPResponseMessage::ACCEPTED);
// 		std::ofstream file;
// 		file.open(path, std::ios::out);
// 		if (file.good()) {
// 			file << request.getBody();
// 			config->created_files[request.getPath()] = path;
// 		}
// 		file.close();
// 	}
// }

// void DELETE_handler( Request request, HTTPResponseMessage& response, webserv::httpData* config, webserv::locationData location ) {
// 	std::map<std::string, std::string>::iterator fileToBeDeleted = config->created_files.find(request.getPath());
// 	if (fileToBeDeleted != config->created_files.end()) {
// 		if (std::remove(fileToBeDeleted->second.c_str()) == 0)
// 			config->created_files.erase(fileToBeDeleted);
// 		else
// 			response.addStatus(HTTPResponseMessage::INTERNAL_SERVER_ERROR);
// 	} else {
// 		response.addStatus(HTTPResponseMessage::INTERNAL_SERVER_ERROR);
// 	}
// }

int findRequestedLocation( webserv::httpData* config, std::vector<std::string> path ){
    int len = 0;
    for ( int i = 0; i < config->locations.size(); i++ ){
        len = config->locations[i].path.size();
        if ( len > path.size() )
            continue;
        for ( int token = len; token >= 0; token-- ){
            if ( config->locations[i].path[token] != path[token] )
                break;
            if ( token == 0 )
                return i;
        }
    }
    return NOTFOUND;
}

HTTPResponseMessage handler( Request request, webserv::httpData* config ) {
	HTTPResponseMessage response;
	int location_index = findRequestedLocation( config, request.getPath() );
	if ( location_index == NOTFOUND )
        (void)location_index; // TODO:: do something

    webserv::locationData location = config->locations[location_index];

	if ( request.getMethod() == Request::GET )
		GET_handler( request.getRequestPath(), response, config, &location );
	// else if ( request.getMethod() == Request::POST )
	// 	POST_handler( request, response, config, location );
	// else if ( request.getMethod() == Request::DELETE )
	// 	DELETE_handler( request, response, config, location );
	return response;
}