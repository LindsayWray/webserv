#include "Request.hpp"

webserv::Request::Request(std::string req){
	std::vector<std::string> header_lines;

	std::stringstream ss(req);

	ss >> _method;
	ss >> _path;
	ss >> _version;
	parse_statusline();

	std::string header;
	while( std::getline(ss, header) ){
		std::stringstream line(header);
		if (header.empty())
			break;
		std::string key;
		std::getline(line, key, ':');
		std::getline(line, _headers[key]);
		if ( key.empty() || _headers[key].empty() || _headers[key].find(':') != std::string::npos ){
			throw(IncorrectRequestException());
		}
	}
	_body = ss.str();

}

void webserv::Request::parse_statusline(){
	if ( _method != "GET" && _method != "POST" && _method != "DELETE" ){
		throw(IncorrectRequestException());
	}

	if ( _path[0] != '/' ){
		throw(IncorrectRequestException());
	}

	if ( _version != "HTTP/1.1" ){
		throw(IncorrectRequestException());
	}
}