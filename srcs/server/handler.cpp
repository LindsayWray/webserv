#include "server.hpp"
#include <iostream>
#include <string>
#include <sys/stat.h>

using webserv::Request;

static std::string file_extension(std::string path) {
	int pos = path.find('.');
	if (pos == std::string::npos) 
		return "";
	return path.substr(pos + 1); // +1 to skip the .
}

static HTTPResponseMessage responseFromFile(std::ifstream& file, std::string extension, HTTPResponseMessage::e_responseStatusCode statusCode) {
	HTTPResponseMessage response;
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
	return response;
}

HTTPResponseMessage fileForbidden( webserv::httpData* config ){
	HTTPResponseMessage response;
	std::string body = "403 Forbidden";
	std::string path;
	std::ifstream file;

	if (config->error_page.find(403) != config->error_page.end()){
		path = config->getRequestedFilePath(config->error_page[403]);
		std::cout << "Get error page " << path << std::endl;
		file.open(path);
		if (file.good()) {
			std::cout << "Error file found " << path << std::endl;
			return responseFromFile(file, file_extension(path), HTTPResponseMessage::FORBIDDEN);
		}
	}
	std::cout << "FILE not found " << path << std::endl;
	return response.addStatus(HTTPResponseMessage::FORBIDDEN)
				.addLength(body.length())
				.addBody(body)
				.addType("text/html");
};

HTTPResponseMessage fileNotFound(webserv::httpData* config ){
	HTTPResponseMessage response;
	std::string body = "404 Not Found";
	std::string path;
	std::ifstream file;

	if (config->error_page.find(404) != config->error_page.end()){
		path = config->getRequestedFilePath(config->error_page[404]);
		std::cout << "Get error page " << path << std::endl;
		file.open(path);
		if (file.good()) {
			std::cout << "Error file found " << path << std::endl;
			return responseFromFile(file, file_extension(path), HTTPResponseMessage::NOT_FOUND);
		}
	}
	std::cout << "FILE not found " << path << std::endl;
	return response.addStatus(HTTPResponseMessage::NOT_FOUND)
				.addLength(body.length())
				.addBody(body)
				.addType("text/html");
};

HTTPResponseMessage GET_handler( std::string path, webserv::httpData* config, webserv::locationData* location ) {
	std::ifstream file;
	HTTPResponseMessage response;
	std::string extension = file_extension(path);
	std::cout << "EXTENSION: "  << extension << std::endl;
	std::string fullPath = location->root + path;

	std::cout << location->root << " " << path << " " << fullPath << "\n";

	if (path.back() == '/') {
		std::cout << "Is a directory " << path << std::endl;
		if (location->autoindex) {
			std::string body;
			try {
				std::string concatPath;
				for(int i = 0; i < location->path.size() - 1; i++){
					concatPath += location->path[i];
				}
				concatPath += path;
				autoIndexing(concatPath, fullPath, body);
			}
			catch ( DirectoryNotFoundException& e){
				std::cout << e.what() << std::endl;
				return fileNotFound(config);
			}	
			return response.addStatus(HTTPResponseMessage::OK)
				.addBody(body)
				.addLength(body.length())
				.addType("text/html");
		}
		return GET_handler(path + "index.html", config, location);
	}

//	if ( location && location->CGI )
//		return responseFromCGI( config, location );

	file.open( fullPath );
	if (file.good()) {
		std::cout << "File found " << fullPath << std::endl;
		return responseFromFile( file, extension, HTTPResponseMessage::OK );
	} else {
		struct stat buf;
		if ( ::stat(fullPath.c_str(), &buf) == -1 ){
			return fileNotFound( config );
		}
		return fileForbidden( config );
	}
}

HTTPResponseMessage POST_handler( std::string& requestPath, Request request, webserv::httpData* config, webserv::locationData* location ) {
	HTTPResponseMessage response;
	std::string extension = file_extension(requestPath);
	std::cout << "EXTENSION: "  << extension << std::endl;
	std::string fullPath = location->root + requestPath;

	struct stat buf;
	bool fileAlreadyExists = (::stat(fullPath.c_str(), &buf) != -1);
	if (fileAlreadyExists) {
		response.addStatus(HTTPResponseMessage::METHOD_NOT_ALLOWED)
				.addType("text/plain")
				.addLength(0)
				.addBody("");
	}
	else {
		response.addStatus(HTTPResponseMessage::ACCEPTED)
				.addLength(0)
				.addBody("");
		std::ofstream file;
		file.open(fullPath, std::ios::out);
		if (file.good()) {
			file << request.getBody();
			config->created_files[request.getRequestPath()] = fullPath;
		}
		file.close();

		std::map<std::string, const std::string>::const_iterator type = HTTPResponseMessage::contentTypes.find(extension);
		if (type != HTTPResponseMessage::contentTypes.end())
			response.addType(type->second);
		else
			response.addType("executable");
	}
	return response;
}

//
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


HTTPResponseMessage REDIRECT_handler( Request request, webserv::httpData* config ) {
    HTTPResponseMessage response;
    std::string requestPath;
    std::string location = config->redirect.second;
    int pos = location.find_first_of("$uri" );
    if ( pos != std::string::npos ) {
        location.erase(pos, 4);
        for (int i = 0; i < request.getPath().size(); i++)
            location.append( request.getPath()[i] );
    }
    response.addStatus( static_cast<HTTPResponseMessage::e_responseStatusCode>(config->redirect.first) )
            .addLength(0)
            .addBody("")
            .addLocation( location );
    return response;
}

HTTPResponseMessage handler( Request request, webserv::httpData* config, webserv::locationData location ) {
	HTTPResponseMessage response;
    std::string requestPath;
    for(int i = location.path.size() - 1; i < request.getPath().size(); i++){
        requestPath += request.getPath()[i];
    }

    if ( config->redirect.first > 0 )
        return REDIRECT_handler( request, config );
	if ( request.getMethod() == Request::GET )
        return GET_handler( requestPath, config, &location );
	else if ( request.getMethod() == Request::POST )
		return POST_handler( requestPath, request, config, &location );
	// else if ( request.getMethod() == Request::DELETE )
	// 	DELETE_handler( request, response, config, location );
	return response;
}