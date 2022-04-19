#include "configParser.hpp"

using namespace webserv;

int configParser::setSocket( socketData& socketData, httpData& httpData ) {
    int port;
    if ( _isWrongInput( NULL ) ) {
        _errorCode = SOCKET;
        return ERROR;
    }
    try {
        port = stoi( * _it++ );
    } catch ( std::exception& e ) { // TODO:: test to see if "iterator out of bounds" will be catched properly;
        std::cerr << "configParser::setSocket " << * _it << " " << e.what() << std::endl;
        _errorCode = SOCKET;
        return ERROR;
    }
    socketData.addPort( port );
    httpData.port = port;
    return _endOfLine( SOCKET );
}

int configParser::setServerName(
        httpData& httpData ) { // TODO:: iterating untill ";" will give false positive in case of no ";" in file
    if ( _isWrongInput( NULL ) ) {
        _errorCode = SERVERNAME;
        return ERROR;
    }
    for ( ; _it != _tokens.end() && * _it != ";"; _it++ )
        httpData.serverName.push_back( * _it );
    return _endOfLine( SERVERNAME );
}

int configParser::setRedirect( httpData& httpData ) {
    int code;
    if ( _isWrongInput( NULL ) ) {
        _errorCode = REDIRECT;
        return ERROR;
    }
    if ( httpData.redirect.first != -1 ) {
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
    httpData.redirect = std::make_pair( code, ( * _it++ ) );
    if ( httpData.redirect.second.find( "$uri" ) != std::string::npos )
        if ( httpData.redirect.second.find( "$uri" ) + 4 < httpData.redirect.second.size() ) {
            _errorCode = REDIRECT;
            return ERROR;
        }
    return _endOfLine( REDIRECT );
}

int configParser::setErrorPage( httpData& httpData ) {
    int error_code;
    std::string filepath, line, body;
    std::ifstream file;

    if ( _isWrongInput( NULL ) ) {
        _errorCode = ERRORPAGE;
        return ERROR;
    }
    for ( ; _it != _tokens.end() && * _it != ";"; _it++ ) {
        filepath = httpData.absPath;
        try {
            error_code = std::stoi( * _it );
        } catch ( std::exception& e ) {
            std::cerr << "configParser::setErrorPage " << * _it << " " << e.what() << std::endl;
            _errorCode = ERRORPAGE;
            return ERROR;
        }
        if ( !_isCorrectCode( error_code ) ) {
            _errorCode = ERRORPAGE;
            return ERROR;
        }
        filepath.append( "/var/error_pages/" );
        filepath.append( * ++_it );
        file.open( filepath );
        if ( file.good() ) {
            while ( std::getline( file, line ) )
                body += ( line + '\n' );
            file.close();
        } else {
            std::cerr << "Error file not found " << filepath << std::endl;
            _errorCode = ERRORPAGE;
            return ERROR;
        }
        httpData.errorPage.insert( std::make_pair( error_code, body ) );
        body.clear();
    }
    return _endOfLine( ERRORPAGE );
}

int configParser::setClientMaxBodySize( httpData& httpData ) {
    if ( _isWrongInput( NULL ) ) {
        _errorCode = MAXBODY;
        return ERROR;
    }
    try {
        httpData.maxClientBody = stoi( * _it++ );
    } catch ( std::exception& e ) {
        std::cerr << "configParser::setClientMaxBodySize " << * _it << " " << e.what() << std::endl;
        _errorCode = MAXBODY;
        return ERROR;
    }
    if ( httpData.maxClientBody <= 0 || httpData.maxClientBody >= 1000000 ) {
        _errorCode = MAXBODY;
        return ERROR;
    }
    return _endOfLine( MAXBODY );
}
