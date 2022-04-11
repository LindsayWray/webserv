//
// Created by Kester kas De rooij on 4/6/22.
//

#include "configParser.hpp"

int webserv::configParser::setLocation( httpData *httpData ) {
    int ret = SUCCES;
    webserv::locationData element( httpData->abs_path );
    ret = _setLocation( element );
    if ( ret == SUCCES && *_it == "{" ) {
        while ( ++_it != _tokens.end() && *_it != "}" ) {
            if ( *_it == "root" )
                ret = _setRoot( element );
            else if ( *_it == "limit_method" )
                ret = _setLimitedMethod( element );
            else if ( *_it == "autoindex" )
                ret = _setAutoindex( element );
            else if ( *_it == "cgi_param" )
                ret = _setCgiParam( element );
            else
                ret = ERROR;
            if ( ret )
                return ret;
        }
    } else
        return ERROR;
    httpData->locations.push_back( element );
    return ret;
}

int webserv::configParser::_setLocation( locationData &element ) {
    if ( _isWrongInput( "{" )){
        _errorCode = LOCATION;
        return ERROR;
    }
    return element.tokenizer( *_it++ );
}

int webserv::configParser::_setRoot( locationData &element ) {
    if ( _isWrongInput( ";" )){
        _errorCode = ROOT;
        return ERROR;
    }
    element.root.append( *_it++ );
    return _endOfLine( ROOT );
}

int webserv::configParser::_setCgiParam( locationData &element ) {
    if ( _isWrongInput( ";" )){
        _errorCode = CGIPARAM;
        return ERROR;
    }
    if ( element.cgi_param != "NONE" ){
        _errorCode = CGIPARAM;
        return ERROR;
    }
    element.cgi_param = *_it++;
    element.CGI = true;
    return _endOfLine( LIMITEDMETHOD );
}

int webserv::configParser::_setLimitedMethod( locationData &element ) {
    if ( _isWrongInput( NULL )){
        _errorCode = LIMITEDMETHOD;
        return ERROR;
    }
	memset( element.allowed_response, false, 3 );

    for ( int i = 0; i < 3 && *_it != ";" ; i++ ){
        if ( *_it == "GET" )
            element.allowed_response[webserv::Request::GET] = true;
        else if ( *_it == "POST" )
            element.allowed_response[webserv::Request::POST] = true;
        else if ( *_it == "DELETE" )
            element.allowed_response[webserv::Request::DELETE] = true;
        else {
            _errorCode = LIMITEDMETHOD;
            return ERROR;
        }
        _it++;
    }
    return _endOfLine( LIMITEDMETHOD );
}

int webserv::configParser::_setAutoindex( locationData &element ) {
    if ( _isWrongInput( ";" )){
        _errorCode = AUTOINDEX;
        return ERROR;
    }
    if ( *_it == "on" )
        element.autoindex = true;
    else if ( *_it == "off" )
        element.autoindex = false;
    else {
        _errorCode = AUTOINDEX;
        return ERROR;
    }
    _it++;
    return _endOfLine( AUTOINDEX );
}
