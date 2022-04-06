//
// Created by Kester kas De rooij on 4/6/22.
//
#include "configParser.hpp"

webserv::FileType &webserv::configParser::getFile( void ) {
    return _configFile;
}

webserv::TokenType webserv::configParser::getTokens( void ) {
    return _tokens;
}

bool webserv::configParser::_isWrongInput( char *str ) {
    if ( ++_it == _tokens.end())
        return true;
    if ( *_it == "{" )
        return true;
    if ( *_it == "}" )
        return true;
    if ( *_it == ";" )
        return true;
    if ( str && *( _it + 1 ) != str )
        return true;
    return false;
}

int webserv::configParser::checkErrorCode( void ){
    switch ( _errorCode ){
        case NONE:
            return SUCCES;
        case NOFILE:
            std::cerr << "File error: No File" << std::endl; break;
        case BADFILE:
            std::cerr << "File error: Bad File" << std::endl; break;
        case EMPTYFILE:
            std::cerr << "File error: Empty File" << std::endl; break;
        case SOCKET:
            std::cerr << "Parsing error: Port definition" << std::endl; break;
        case WORKERC:
            std::cerr << "Parsing error: Worker connections" << std::endl; break;
        case INDEX:
            std::cerr << "Parsing error: Index definition" << std::endl; break;
        case MAXBODY:
            std::cerr << "Parsing error: Set_max_client_body definition" << std::endl; break;
        case LOCATION:
            std::cerr << "Parsing error: Location definition" << std::endl; break;
        case SERVERNAME:
            std::cerr << "Parsing error: Server_name definition" << std::endl; break;
        case ERRORPAGE:
            std::cerr << "Parsing error: Error_page definition" << std::endl; break;
        case REDIRECT:
            std::cerr << "Parsing error: Redirect/return definition" << std::endl; break;
        case ROOT:
            std::cerr << "Parsing error: Root definition" << std::endl; break;
        case CGIPARAM:
            std::cerr << "Parsing error: CGI_param definition" << std::endl; break;
        case ALLOWEDRESPONSE:
            std::cerr << "Parsing error: Add_header definition" << std::endl; break;
        case AUTOINDEX:
            std::cerr << "Parsing error: Autoindex definition" << std::endl; break;
    }
    return ERROR;
}
