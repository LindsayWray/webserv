#include "configParser.hpp"

using namespace webserv;

int configParser::setSocket( socketData& socketData, httpData& httpData ) {
    int port;
    if ( _isWrongInput( "" ) || httpData.port ) {
        _errorCode = SOCKET;
        return ERROR;
    }
    try {
        port = stoi( * _it++ );
    } catch ( std::exception& e ) {
        std::cerr << "configParser::setSocket " << * _it << " " << e.what() << std::endl;
        _errorCode = SOCKET;
        return ERROR;
    }
    if ( port <= 0 || port > 65535 ){
        _errorCode = SOCKET;
        return ERROR;
    }
    socketData.addPort( port );
    httpData.port = port;
    return _endOfLine( SOCKET );
}

int configParser::setServerName( httpData& httpData ) {
    if ( _isWrongInput( "" ) ) {
        _errorCode = SERVERNAME;
        return ERROR;
    }
    for ( ; _it != _tokens.end() && * _it != ";"; _it++ )
        httpData.serverName.push_back( * _it );
    return _endOfLine( SERVERNAME );
}

int configParser::setErrorPage( httpData& httpData ) {
    int error_code;
    std::string filepath, line, body;
    std::ifstream file;

    if ( _isWrongInput( "" ) ) {
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
    if ( _isWrongInput( "" ) ) {
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
