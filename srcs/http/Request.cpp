#include "Request.hpp"
#include "../utils/printFormatting.hpp"
#include <fstream> 

webserv::Request::Request(int max_client_body){
	_max_client_body = max_client_body;
	_headersDone = false; 
};

webserv::Request::Request(const Request& original){
	*this = original;
}

webserv::Request& webserv::Request::operator=(const webserv::Request& original){
	this->_rawRequest = original._rawRequest;
	this->_headersDone = original._headersDone;
	this->_contentLength = original._contentLength;
	this->_max_client_body = original._max_client_body;
	this->_method = original._method;
	this->_path = original._path;
	this->_requestPath = original._requestPath;
	this->_version = original._version;
	this->_headers = original._headers;
	this->_body = original._body;
	this->_chunked = original._chunked;
	this->_chunkedComplete = original._chunkedComplete;
	this->_remainder = original._remainder;
	return *this;
}

void webserv::Request::parse_statusline( std::string &method ) {
    if ( method == "GET" )
        _method = GET;
    else if ( method == "POST" )
        _method = POST;
    else if ( method == "DELETE" )
        _method = DELETE;
    else {
        printf( "Fault in the statusline, (method)\n" );
        throw ( IncorrectRequestException());
    }

    if ( _path[0] != "/" ) {
        printf( "Fault in the statusline, (path)\n" );
        throw ( IncorrectRequestException());
    }

    if ( _version != "HTTP/1.1" ) {
        printf( "Fault in the statusline, (version)\n" );
        throw ( IncorrectRequestException());
    }
}

void webserv::Request::setPath( std::string line ) {
	std::size_t i = 0, found;

	if ( line[i] != '/' )
		return;
	while ( i < line.length()) {
		found = line.find_first_of( "/", i );
		if ( found == std::string::npos ) {
			_path.push_back( line.substr( i, line.length()));
			break;
		} else {
			if ( i != found - i )
				_path.push_back( line.substr( i, found - i ));
			_path.push_back( line.substr( found, 1 ));
			i = found + 1;
		}
	}
}

void	webserv::Request::appendBody(const char* chunk, int len) {
	std::cout << "appending" << _chunked << std::endl ; 
	if (!_chunked)
		_body.append(chunk, len);
		if (_body.size() > _contentLength)
			throw (IncorrectRequestException());
	else {
		//Chunk logic
		int size;
		char *buf;

		std::stringstream ss(chunk);
		if (_body.empty())
			ss >> size; // skip the first int, its weird

		// if (_remainder) {
		// 	buf = (char *)malloc(_remainder + 1);
		// 	ss.get(buf, _remainder + 1);
		// 	_body.append(buf, _remainder);
		// 	_remainder = 0;
		// 	free(buf);
		// }
	
		while (size != 0) {
			ss >> std::hex >> size;
			if (size == 0) {
				_chunkedComplete = true;
				return;
			}
			// std::cout << "Chunk size" << size << std::endl;
			ss.ignore(1); 
			buf = (char *)malloc(size + 1);
			// std::cout << ss.tellg() << std::endl;
			ss.get(buf, size + 1);
			// if (strlen(buf) != size) {
			// 	_body.append(buf, strlen(buf));
			// 	_remainder = size - strlen(buf);
			// 	free(buf);
			// 	break;
			// }
			// std::cout << ss.tellg() << std::endl;
			// ss.ignore(1);
			_body.append(buf, size + 1);
			// std::cout << _body << std::endl;
			free(buf);
		}
	}
}

void	webserv::Request::parseChunk(char* chunk, int len){

	if (_headersDone){
		_body.append(chunk, len);
		if (_body.size() > _contentLength)
			throw ( IncorrectRequestException() );
	}
	else
		_rawRequest.append(chunk, len);

	if (_rawRequest.find( "\r\n\r\n" ) != std::string::npos && !_headersDone) {
	    std::string method;

		std::stringstream ss( _rawRequest );
		ss >> method;
		ss >> _requestPath;
		ss >> _version;
		ss.ignore( 2 );

		setPath( _requestPath );
		parse_statusline( method );

		std::string header;
		while ( std::getline( ss, header )) {
			std::stringstream line( header );
			if ( header.empty() || header == "\r" )
				break;
			std::string key;
			std::getline( line, key, ':' );
			std::getline( line, _headers[key] );
			if ( key.empty() || _headers[key].empty()) {
				printf( "Fault in the headers\n" );
				throw ( IncorrectRequestException());
			}
		}

		if (_headers.find("Content-Length") == _headers.end())
			_contentLength = 0; 
		else 
			_contentLength = std::stoi(_headers["Content-Length"]);
		
		std::cout << "MAX BODY " << _max_client_body << std::endl;
		if ( _max_client_body != 0 && _contentLength > _max_client_body )
			throw (MaxClientBodyException());

		int current_position = ss.tellg();
		_body = _rawRequest.substr(current_position, _rawRequest.size() - current_position );
		_headersDone = true;
	}
}

std::string webserv::Request::getBody() const {
    return this->_body;
}

std::vector<std::string> webserv::Request::getPath() const {
    return this->_path;
}

std::string webserv::Request::getRequestPath() const {
    return this->_requestPath;
}

webserv::Request::method webserv::Request::getMethod() const {
    return this->_method;
}

std::string webserv::Request::getRawRequest() const {
    return this->_rawRequest;
}

bool webserv::Request::isComplete() const {
	if (!_headersDone)
		return false;
	if (_chunked)
		return ( _chunkedComplete );
	std::cout << "Checking..." << this->_body.size() << " - " << this->_contentLength << std::endl;
	return (this->_body.size() >= this->_contentLength);
}