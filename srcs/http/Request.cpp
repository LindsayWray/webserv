#include "Request.hpp"
#include "../utils/printFormatting.hpp"

webserv::Request::Request( std::string req ) {
    std::vector<std::string> header_lines;

    std::stringstream ss( req );
    std::string method;

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
    std::getline( ss, _body, ( char ) 26 );

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



// The normal procedure for parsing an HTTP message is to read the
//    start-line into a structure, read each header field into a hash table
//    by field name until the empty line, and then use the parsed data to
//    determine if a message body is expected.  If a message body has been
//    indicated, then it is read as a stream until an amount of octets
//    equal to the message body length is read or the connection is closed.

//    A recipient MUST parse an HTTP message as a sequence of octets in an
//    encoding that is a superset of US-ASCII [USASCII].  Parsing an HTTP
//    message as a stream of Unicode characters, without regard for the
//    specific encoding, creates security vulnerabilities due to the
//    varying ways that string processing libraries handle invalid
//    multibyte character sequences that contain the octet LF (%x0A).
//    String-based parsers can only be safely used within protocol elements
//    after the element has been extracted from the message, such as within
//    a header field-value after message parsing has delineated the
//    individual fields.

//    An HTTP message can be parsed as a stream for incremental processing
//    or forwarding downstream.  However, recipients cannot rely on
//    incremental delivery of partial messages, since some implementations
//    will buffer or delay message forwarding for the sake of network
//    efficiency, security checks, or payload transformations.

//    A sender MUST NOT send whitespace between the start-line and the
//    first header field.  A recipient that receives whitespace between the
//    start-line and the first header field MUST either reject the message
//    as invalid or consume each whitespace-preceded line without further
//    processing of it (i.e., ignore the entire line, along with any
//    subsequent lines preceded by whitespace, until a properly formed
//    header field is received or the header section is terminated).

//    The presence of such whitespace in a request might be an attempt to
//    trick a server into ignoring that field or processing the line after
//    it as a new request, either of which might result in a security
//    vulnerability if other implementations within the request chain
//    interpret the same message differently.  Likewise, the presence of
//    such whitespace in a response might be ignored by some clients or
//    cause others to cease parsing.



