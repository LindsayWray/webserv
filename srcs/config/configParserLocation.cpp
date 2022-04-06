//
// Created by Kester kas De rooij on 4/6/22.
//

#include "configParser.hpp"

static void _insertBefore( webserv::httpData *httpData, webserv::locationData &element ) {
    std::vector<webserv::locationData>::iterator it = --httpData->locations.end();
    for ( ; it >= httpData->locations.begin(); it-- ) {
        if ( element.path.size() >= it->path.size())
            httpData->locations.insert( it, element );
    }
}

int webserv::configParser::setLocation( httpData *httpData ) {
    int ret = SUCCES;
    webserv::locationData element( httpData->abs_path );
    ret = _setLocation( element );
    if ( ret == SUCCES && *_it == "{" ) {
        while ( ++_it != _tokens.end() && *_it != "}" ) {
            if ( *_it == "root" )
                ret = _setRoot( element );
            else if ( *_it == "add_header" )
                ret = _setAllowedResponse( element );
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
    if ( httpData->locations.size() > 0 && element.path.size() > httpData->locations.back().path.size())
        _insertBefore( httpData, element );
    else
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
    element.root.append( *_it );
    for ( ; _it != _tokens.end() && *( _it + 1 ) == ";"; _it++ ) {}
    return SUCCES;
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
    element.cgi_param = *_it;
    for ( ; _it != _tokens.end() && *( _it + 1 ) == ";"; _it++ ) {}
    element.CGI = true;
    return SUCCES;
}

int webserv::configParser::_setAllowedResponse( locationData &element ) {
    if ( _isWrongInput( ";" )){
        _errorCode = ALLOWEDRESPONSE;
        return ERROR;
    }

    // TODO:: figure right wy to format this
//	if ( *_it != "GET" ) )
//		element.allowed_response[GET] = true;

    return SUCCES;
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
    for ( ; _it != _tokens.end() && *( _it + 1 ) == ";"; _it++ ) {}
    return SUCCES;
}