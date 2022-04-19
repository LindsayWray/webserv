#include "Request.hpp"
#include "../utils/printFormatting.hpp"
#include <fstream>
#include <cmath> //added for transfer encoding: pow();

using namespace webserv;

Request::Request(int max_client_body){
	_maxClientBody = max_client_body;
	_remainder = 0;
	_chunkEndsWithHex = false;
	_chunkEndsWithSeparatedCRLF = false;
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
	this->_chunkEndsWithHex = original._chunkEndsWithHex;
	this->_chunkEndsWithSeparatedCRLF = original._chunkEndsWithSeparatedCRLF;
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
	// else if ( method == "PUT" )
		// _method = POST;
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
		long size;
		int i = 0;

		if (_chunkEndsWithHex) {
			while (chunk[i] != '\r') {
				if (chunk[i] >= '0' && chunk[i] <= '9')
					_remainder = _remainder * 16 + chunk[i] - '0';
				else if (chunk[i] >= 'a' && chunk[i] <= 'f')
					_remainder = _remainder * 16 + chunk[i] - 'a' + 10;
				else if (chunk[i] >= 'A' && chunk[i] <= 'F')
					_remainder = _remainder * 16 + chunk[i] - 'A' + 10;
				i++;
			}
			i += 2;
			_chunkEndsWithHex = false;
		} else if (_chunkEndsWithSeparatedCRLF) {
			i += 1;
			_chunkEndsWithSeparatedCRLF = false;
		}

		//std::cout << "Remaining" << _remainder << std::endl;
		if (_remainder != 0) {
			if (_remainder > len - i)
			{
				_body.append(chunk + i, len - i);
				_remainder -= (len - i);
				return;
			} else if (_remainder + 1 > len - i) {
				_body.append(chunk + i, _remainder);
				_remainder = 0;
				_chunkEndsWithHex = true;
				return;
			} else if (_remainder + 2 >= len - i) {
				_body.append(chunk + i, _remainder);
				_remainder = 0;
				_chunkEndsWithSeparatedCRLF = true;
				return;
			}
			else {
				_body.append(chunk + i, _remainder);
				i += _remainder;
				i += 2; // \r\n
				_remainder = 0;
			}
		}
	
		while (i < len ) {
			//std::cout << "PEEK " << (int)chunk[i] << std::endl;
			size = 0;
			if (chunk[i] == '0'){
				_chunkedComplete = true;
				return;
			}
			char* end;
			size = strtol( chunk + i, &end, 16 );

			//std::cout << "Chunk size" << size << std::endl;
			int hexLength = (end - (chunk + i));

			_chunkEndsWithHex = (i + hexLength == len);
			_chunkEndsWithSeparatedCRLF = (i + hexLength + 1 == len);
			if (_chunkEndsWithHex) {					// e.g. "...0x7AF\0"
				_remainder = size;
				return;
				// hex possibly not fully in chunk, save hex string digits for next chunk
			}
			else if ( _chunkEndsWithSeparatedCRLF ) { 	// e.g. "...0x7AF\r\0"
				_remainder = size;
				return;
				// skip first character of next chunk
			}
			else {
				i += hexLength;
				i += 2; // \r\n
			}

			if ( _maxClientBody != 0 && _body.size() + size > _maxClientBody )
				throw (MaxClientBodyException());

			int charsLeft = len - i;
			//std::cout << "Chars left" << charsLeft << std::endl;

			if (charsLeft >= size + 2) {
				_body.append(chunk + i, size);
				i += size;
				i += 2; // \r\n
			} else if (charsLeft >= size + 1) {
				_body.append(chunk + i, size);
				_remainder = 0;
				_chunkedComplete = false;
				_chunkEndsWithSeparatedCRLF = true;
				return;
			} else if (charsLeft >= size) {
				_body.append(chunk + i, size);
				_remainder = 0;
				_chunkedComplete = false;
				_chunkEndsWithHex = true;
				return;
			}
			else {
				_body.append(chunk + i, charsLeft);
				_remainder = size - charsLeft;
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
	//std::cout << "Checking..." << this->_body.size() << " - " << this->_contentLength << std::endl;
	return (this->_body.size() >= this->_contentLength);
}
