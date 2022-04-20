#include "configParser.hpp"

using namespace webserv;

int webserv::configParser::setLocation( httpData& httpData ) {
    int ret = SUCCES;
    locationData element( httpData.absPath );
    ret = _setLocation( element );
    if ( ret >= SUCCES && * _it == "{" ) {
        while ( ++_it != _tokens.end() && * _it != "}" ) {
            if ( * _it == "root" )
                ret = _setRoot( element );
            else if ( * _it == "limit_method" )
                ret = _setLimitedMethod( element );
            else if ( * _it == "index" )
                ret = _setIndex( element );
            else if ( * _it == "autoindex" )
                ret = _setAutoindex( element );
            else if ( * _it == "cgi_param" )
                ret = _setCgiParam( element );
            else if ( * _it == "return" )
                ret = _setRedirect( element );
            else
                ret = ERROR;
            if ( ret )
                return ret;
        }
//    } else if ( ret == CGI_SIGN && * _it == "{" ){
//        while ( ++_it != _tokens.end() && * _it != "}" ) {
//            if ( * _it == "root" )
//                ret = _setRoot( element );
//            else if ( * _it == "limit_method" )
//                ret = _setLimitedMethod( element );
//            else if ( * _it == "index" )
//                ret = _setIndex( element );
//            else if ( * _it == "autoindex" )
//                ret = _setAutoindex( element );
//            else if ( * _it == "cgi_param" )
//                ret = _setCgiParam( element );//element.tokenizer( * (_it + 1) ) == SUCCES ? _setCgiParam( element ) : ERROR;
//            else
//                ret = ERROR;
//            if ( ret )
//                return ret;
//        }
    } else {
        _errorCode = LOCATION;
        return ERROR;
    }
    if ( element.index.empty() )
        element.index = "index.html";
    httpData.locations.push_back( element );
    return ret;
}

int webserv::configParser::_setLocation( locationData& element ) {
    if ( _isWrongInput( "{" ) ) {
        _errorCode = LOCATION;
        return ERROR;
    }
    if ( (*_it)[0] == '\\' ){
        if ( *_it++ != "\\.py$" ) {
            _errorCode = LOCATION;
            return ERROR;
        }
        element.path.push_back( "\\.py$" );
        element.CGI = true;
        return CGI_SIGN;
    }
    return element.pathTokenizer( * _it++ );
}

int webserv::configParser::_setIndex( locationData& element ) {
    if ( _isWrongInput( ";" ) ) {
        _errorCode = INDEX;
        return ERROR;
    }
    if ( element.index.empty() )
        element.index = * _it++;
    else {
        _errorCode = INDEX;
        return ERROR;
    }
    return _endOfLine( INDEX );
}

int webserv::configParser::_setRoot( locationData& element ) {
    if ( _isWrongInput( ";" ) ) {
        _errorCode = ROOT;
        return ERROR;
    }
    element.root.append( * _it++ );
    return _endOfLine( ROOT );
}

int webserv::configParser::_setCgiParam( locationData& element ) {
    if ( _isWrongInput( ";" ) ) {
        _errorCode = CGIPARAM;
        return ERROR;
    }
    if ( !element.cgi_param.empty() ) {
        _errorCode = CGIPARAM;
        return ERROR;
    }
    element.cgiTokenizer( * _it++ );
    element.CGI = true;
    return _endOfLine( LIMITEDMETHOD );
}

int webserv::configParser::_setLimitedMethod( locationData& element ) {
    if ( _isWrongInput( NULL ) ) {
        _errorCode = LIMITEDMETHOD;
        return ERROR;
    }
    memset( element.allowed_response, false, 3 );

    for ( int i = 0; i < 3 && * _it != ";"; i++ ) {
        if ( * _it == "GET" )
            element.allowed_response[Request::GET] = true;
        else if ( * _it == "POST" )
            element.allowed_response[Request::POST] = true;
        else if ( * _it == "DELETE" )
            element.allowed_response[Request::DELETE] = true;
        else {
            _errorCode = LIMITEDMETHOD;
            return ERROR;
        }
        _it++;
    }
    return _endOfLine( LIMITEDMETHOD );
}


int configParser::_setRedirect( locationData& element ) {
    int code;
    if ( _isWrongInput( NULL ) ) {
        _errorCode = REDIRECT;
        return ERROR;
    }
    if ( element.redirect.first != -1 ) {
        _errorCode = REDIRECT;
        return ERROR;
    }
    try {
        code = std::stoi( * ( _it++ ) );
    } catch ( std::exception& e ) {
        std::cerr << "configParser::setRedirect " << * _it << " " << e.what() << std::endl;
        _errorCode = REDIRECT;
        return ERROR;
    }
    if ( code != 301 && code != 302 && code != 303 && code != 307 ) {
        std::cerr << "configParser::setRedirect wrong redirection code" << std::endl;
        _errorCode = REDIRECT;
        return ERROR;
    }
    element.redirect = std::make_pair( code, ( * _it++ ) );
    if ( element.redirect.second.find( "$uri" ) != std::string::npos )
        if ( element.redirect.second.find( "$uri" ) + 4 < element.redirect.second.size() ) {
            _errorCode = REDIRECT;
            return ERROR;
        }
    return _endOfLine( REDIRECT );
}

int webserv::configParser::_setAutoindex( locationData& element ) {
    if ( _isWrongInput( ";" ) ) {
        _errorCode = AUTOINDEX;
        return ERROR;
    }
    if ( * _it == "on" )
        element.autoindex = true;
    else if ( * _it == "off" )
        element.autoindex = false;
    else {
        _errorCode = AUTOINDEX;
        return ERROR;
    }
    _it++;
    return _endOfLine( AUTOINDEX );
}