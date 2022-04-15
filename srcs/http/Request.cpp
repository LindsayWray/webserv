#include "Request.hpp"
#include "../utils/printFormatting.hpp"
#include <fstream>

using namespace webserv;

Request::Request(int max_client_body){
	_maxClientBody = max_client_body;
	_remainder = 0;
};

Request::Request(const Request& original){
	*this = original;
}

Request& Request::operator=(const Request& original){
	this->_rawRequest = original._rawRequest;
	this->_headersDone = original._headersDone;
	this->_contentLength = original._contentLength;
	this->_maxClientBody = original._maxClientBody;
	this->_method = original._method;
	this->_path = original._path;
	this->_requestPath = original._requestPath;
	this->_version = original._version;
	this->_headers = original._headers;
	this->_body = original._body;
	this->_chunked = original._chunked;
	this->_chunkedComplete = original._chunkedComplete;
	this->_remainder = original._remainder;
	this->_host = original._host;
	return *this;
}

void Request::parse_statusline( std::string &method ) {
    if ( method == "GET" )
        _method = GET;
    else if ( method == "POST" )
        _method = POST;
    else if ( method == "DELETE" )
        _method = DELETE;
    else {
        printf( "Fault in the statusline, (method)\n" );
        throw ( MethodNotAllowedException());
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

void Request::setPath( std::string line ) {
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

void	Request::appendBody(const char* chunk, int len) {
	if (!_chunked) {
		_body.append(chunk, len);
		if (_body.size() > _contentLength)
			throw (IncorrectRequestException());
	} else {
		int size;
		char *buf;

		std::stringstream ss(chunk);

		std::cout << "Remaining" << _remainder << std::endl;

		if (_remainder != 0) {
			buf = (char *)malloc(_remainder);
			int charsRead = ss.readsome(buf, _remainder);
			_body.append(buf, charsRead);
			_remainder -= charsRead;
			free(buf);
		}
		if (_remainder != 0)
			return;
	
		while (!ss.eof() ) {
			size = 0;
			//std::cout << "PEEK " << ss.peek() << std::endl;
			ss >> std::hex >> size;
			if (size == 0 && !ss.eof()) {
				_chunkedComplete = true;
				return;
			}
			// std::cout << "Chunk size" << size << std::endl;
			ss.ignore(2); 
			buf = (char *)malloc(size);
			int charsRead = ss.readsome(buf, size);
			_body.append(buf, charsRead);
			free(buf);

			if (charsRead < size) {
				// std::cout << "Not enough chars found " << charsRead << "  of " << size << std::endl;
				_remainder = size - charsRead;
				_chunkedComplete = false;
				return;
			}
		}
	}
}

void	Request::decodePath() {
	while(true) {
		int pos = _requestPath.find("%20");
		if (pos == std::string::npos)
			return;
		_requestPath.replace(pos, 3, " ");
	}
}

static std::string lowercase(std::string str) {
	for(int i = 0; i < str.size(); i++) {
		str[i] = tolower(str[i]);
	}
	return str;
}

void	Request::parseChunk(char* chunk, int len){

	if (_headersDone)
		appendBody(chunk, len);
	else
		_rawRequest.append(chunk, len);

	if (_rawRequest.find( "\r\n\r\n" ) != std::string::npos && !_headersDone) {
	    std::string method;

		std::stringstream ss( _rawRequest );
		ss >> method;
		ss >> _requestPath;
		ss >> _version;
		ss.ignore( 2 );

		decodePath();
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
			_headers[lowercase(key)] = _headers[key].substr( 1, _headers[key].size() - 2 );
		}

		if (_headers.find(TRANSFER_ENCODING) != _headers.end() && _headers[TRANSFER_ENCODING] == "chunked")
			_chunked = true;
		else if (_headers.find(CONTENT_LENGTH) == _headers.end())
			_contentLength = 0; 
		else 
			_contentLength = std::stoi(_headers[CONTENT_LENGTH]);

        if (_headers.find(HOST) == _headers.end())
            _host = "NAV";
        else
            _host = _headers[HOST];

		std::cout << "Content Length " << _contentLength << std::endl;
		std::cout << "Chunked" << std::boolalpha << _chunked << std::endl;

		std::cout << "MAX BODY " << _maxClientBody << std::endl;
		if ( !_chunked && _maxClientBody != 0 && _contentLength > _maxClientBody )
			throw (MaxClientBodyException());

		int current_position = ss.tellg();
		std::string rest = _rawRequest.substr(current_position, _rawRequest.size() - current_position );
		appendBody(rest.c_str(), rest.size());
		_headersDone = true;
	}
}

std::string Request::getBody() const {
    return this->_body;
}

std::vector<std::string> Request::getPath() const {
    return this->_path;
}

std::string Request::getRequestPath() const {
    return this->_requestPath;
}

Request::method Request::getMethod() const {
    return this->_method;
}

std::string Request::getRawRequest() const {
    return this->_rawRequest;
}

std::string Request::getHost() const {
    return this->_host;
}


bool Request::isComplete() const {
	if (!_headersDone)
		return false;
	if (_chunked)
		return ( _chunkedComplete );
	std::cout << "Checking..." << this->_body.size() << " - " << this->_contentLength << std::endl;
	return (this->_body.size() >= this->_contentLength);
}