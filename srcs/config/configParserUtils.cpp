#include "configParser.hpp"

using namespace webserv;

FileType& configParser::getFile( void ) {
    return _configFile;
}

TokenType configParser::getTokens( void ) {
    return _tokens;
}

bool configParser::_isCorrectCode( int input ) {
    switch ( input ) {
        case 400:
            return true;
        case 403:
            return true;
        case 404:
            return true;
        case 408:
            return true;
        case 411:
            return true;
        case 413:
            return true;
        case 414:
            return true;
        case 418:
            return true;
        case 429:
            return true;
        case 431:
            return true;
        case 500:
            return true;
        case 501:
            return true;
        case 505:
            return true;
    }
    return false;
}

bool configParser::_isWrongInput( std::string str ) {
    if ( ++_it == _tokens.end() )
        return true;
    if ( * _it == "{" )
        return true;
    if ( * _it == "}" )
        return true;
    if ( * _it == ";" )
        return true;
    if ( !str.empty() && * ( _it + 1 ) != str )
        return true;
    return false;
}

int configParser::_endOfLine( int errorcode ) {
    if ( _it == _tokens.end() || * _it != ";" ) {
        _errorCode = errorcode;
        return ERROR;
    }
    for ( ; _it != _tokens.end() && * ( _it + 1 ) == ";"; _it++ );
    return SUCCES;
}

int configParser::checkErrorCode( void ) {
    switch ( _errorCode ) {
        case NONE:
            return SUCCES;
        case NOFILE:
            std::cerr << "File error: No File" << std::endl;
            break;
        case BADFILE:
            std::cerr << "File error: Bad File" << std::endl;
            break;
        case EMPTYFILE:
            std::cerr << "File error: Empty File" << std::endl;
            break;
        case SERVER:
            std::cerr << "File error: Server definition" << std::endl;
            break;
        case SOCKET:
            std::cerr << "Parsing error: Port definition" << std::endl;
            break;
        case WORKERC:
            std::cerr << "Parsing error: Worker connections" << std::endl;
            break;
        case INDEX:
            std::cerr << "Parsing error: Index definition" << std::endl;
            break;
        case MAXBODY:
            std::cerr << "Parsing error: Set_max_client_body definition" << std::endl;
            break;
        case LOCATION:
            std::cerr << "Parsing error: Location definition" << std::endl;
            break;
        case SERVERNAME:
            std::cerr << "Parsing error: Server_name definition" << std::endl;
            break;
        case ERRORPAGE:
            std::cerr << "Parsing error: Error_page definition" << std::endl;
            break;
        case REDIRECT:
            std::cerr << "Parsing error: Redirect/return definition" << std::endl;
            break;
        case ROOT:
            std::cerr << "Parsing error: Root definition" << std::endl;
            break;
        case CGIPARAM:
            std::cerr << "Parsing error: CGI_param definition" << std::endl;
            break;
        case LIMITEDMETHOD:
            std::cerr << "Parsing error: Add_header definition" << std::endl;
            break;
        case AUTOINDEX:
            std::cerr << "Parsing error: Autoindex definition" << std::endl;
            break;
    }
    return ERROR;
}