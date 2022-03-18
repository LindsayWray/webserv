#include "Request.hpp"

webserv::Request::Request(std::string req){
	std::vector<std::string> header_lines;

	std::stringstream ss(req);
//	std::string		crlf;

	ss >> _method;
	ss >> _path;
	ss >> _version;
	ss.ignore(2);
	
	parse_statusline();
	
	//std::cout << _method << _path << _version << crlf << std::endl;
	std::cout << "Request" << req << std::endl;

	std::string header;
	while( std::getline(ss, header) ){
	//	std::cout << "header: " << header << std::endl;
		std::stringstream line(header);
		if (header.empty() || header == "\r")
			break;
		std::string key;
		std::getline(line, key, ':');
		std::getline(line, _headers[key]);
		//std::cout << "key: " << key << std::endl;
		//std::cout << "value: " <<  _headers[key] << std::endl;
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