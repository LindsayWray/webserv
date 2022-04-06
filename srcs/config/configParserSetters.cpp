//
// Created by Kester kas De rooij on 4/6/22.
//

#include "configParser.hpp"

int webserv::configParser::setSocket( socketData *socketData ) {
    if ( _isWrongInput( NULL )){
        _errorCode = SOCKET;
        return ERROR;
    }
    try {
        socketData->addPort( stoi( *_it ));
    } catch ( std::exception &e ) { // TODO:: test to see if "iterator out of bounds" will be catched properly;
        std::cerr << "configParser::setSocket " << *_it << " " << e.what() << std::endl;
        _errorCode = SOCKET;
        return ERROR;
    }
    if ( _it == _tokens.end() || *( ++_it ) != ";" ){
        _errorCode = SOCKET;
        return ERROR;
    }
    for ( ; _it != _tokens.end() && *( _it + 1 ) == ";"; _it++ ) {}
    return SUCCES;
}

int webserv::configParser::setWorkerConnections( socketData *socketData ) {
    if ( _isWrongInput( NULL )){
        _errorCode = WORKERC;
        return ERROR;
    }
    try {
        socketData->worker_connections = stoi( *( ++_it ));
    } catch ( std::exception &e ) {
        std::cerr << "configParser::setWorkerConnections " << *_it << " " << e.what() << std::endl;
        _errorCode = WORKERC;
        return ERROR;
    }
    if ( _it == _tokens.end() || *( ++_it ) != ";" ){
        _errorCode = WORKERC;
        return ERROR;
    }
    for ( ; _it != _tokens.end() && *( _it + 1 ) == ";"; _it++ ) {}
    return SUCCES;
}

int webserv::configParser::setIndex(
        httpData *httpData ) {
    if ( _isWrongInput( NULL )){
        _errorCode = INDEX;
        return ERROR;
    }
    httpData->index.push_back( *_it++ );
    if ( _it == _tokens.end() || *( _it ) != ";" ){
        _errorCode = INDEX;
        return ERROR;
    }
    for ( ; _it != _tokens.end() && *( _it + 1 ) == ";"; _it++ ) {}
    return SUCCES;
}

int webserv::configParser::setServerName(
        httpData *httpData ) { // TODO:: iterating untill ";" will give false positive in case of no ";" in file
    if ( _isWrongInput( NULL )){
        _errorCode = SERVERNAME;
        return ERROR;
    }
    for ( ; _it != _tokens.end() && *_it != ";"; _it++ )
        httpData->server_name.push_back( *_it );
    if ( _it == _tokens.end() || *( _it ) != ";" ){
        _errorCode = SERVERNAME;
        return ERROR;
    }
    for ( ; _it != _tokens.end() && *( _it + 1 ) == ";"; _it++ ) {}
    return SUCCES;
}

int webserv::configParser::setRedirect( httpData *httpData ) {
    int code;
    if ( _isWrongInput( NULL )){
        _errorCode = REDIRECT;
        return ERROR;
    }
    if ( httpData->redirect.first != -1 ){
        _errorCode = REDIRECT;
        return ERROR;
    }
    try {
        code = std::stoi( *( _it++ ));
    } catch ( std::exception &e ) {
        std::cerr << "configParser::setRedirect " << *_it << " " << e.what() << std::endl;
        _errorCode = REDIRECT;
        return ERROR;
    }
    if ( code != 301 && code != 302 && code != 303 && code != 307 ){
        std::cerr << "configParser::setRedirect wrong redirection code" << std::endl;
        _errorCode = REDIRECT;
        return ERROR;
    }
    httpData->redirect = std::make_pair( code, ( *_it ));
    if ( httpData->redirect.second.find( "$uri" ) != std::string::npos )
        if ( httpData->redirect.second.find( "$uri" ) + 4 < httpData->redirect.second.size()){
            _errorCode = REDIRECT;
            return ERROR;
        }
    if ( _it == _tokens.end() || *( ++_it ) != ";" ){ // TODO:: check different url's to see if some charachters mess up the tokenizer
        _errorCode = REDIRECT;
        return ERROR;
    }
    for ( ; _it != _tokens.end() && *( _it + 1 ) == ";"; _it++ );
    return SUCCES;
}

int webserv::configParser::setErrorPage( httpData *httpData ) {
    int error_code;
    std::string filepath, line, body;
    std::ifstream file;

    if ( _isWrongInput( NULL )){
        _errorCode = ERRORPAGE;
        return ERROR;
    }
    for ( ; _it != _tokens.end() && *_it != ";"; _it++ ) {
        filepath = httpData->abs_path;
        try {
            error_code = std::stoi( *( _it++ ));
        } catch ( std::exception &e ) {
            std::cerr << "configParser::setErrorPage " << *_it << " " << e.what() << std::endl;
            _errorCode = ERRORPAGE;
            return ERROR;
        }
        filepath.append("/var/error_pages/");
        filepath.append(*_it );
        file.open( filepath );
        if ( file.good()) {
            while ( std::getline( file, line ))
                body += ( line + '\n' );
            file.close();
        } else {
            std::cerr << "Error file not found " << filepath << std::endl;
            _errorCode = ERRORPAGE;
            return ERROR;
        }
        httpData->error_page.insert( std::make_pair( error_code, body));
    }
    for ( ; _it != _tokens.end() && *( _it + 1 ) == ";"; _it++ );
    return SUCCES;
}

int webserv::configParser::setClientMaxBodySize( webserv::httpData *httpData ) {
    if ( _isWrongInput( NULL )){
        _errorCode = MAXBODY;
        return ERROR;
    }
    try {
        httpData->max_client_body_size = stoi( *_it );
    } catch ( std::exception &e ) {
        std::cerr << "configParser::setClientMaxBodySize " << *_it << " " << e.what() << std::endl;
        _errorCode = MAXBODY;
        return ERROR;
    }
    if ( httpData->max_client_body_size <= 0 || httpData->max_client_body_size >= 1000000 ){
        _errorCode = MAXBODY;
        return ERROR;
    }
    if ( _it == _tokens.end() || *( ++_it ) != ";" ){
        _errorCode = MAXBODY;
        return ERROR;
    }
    for ( ; _it != _tokens.end() && *( _it + 1 ) == ";"; _it++ ) {}
    return SUCCES;
}
