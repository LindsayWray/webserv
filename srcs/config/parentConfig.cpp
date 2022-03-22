//
// Created by Kester kas De rooij on 3/16/22.
//

#include "parentConfig.hpp"

webserv::parentConfig::parentConfig( std::string config_file ) {
	_configFile.open( config_file, std::fstream::in );
	if ( _configFile.fail() )
		std::cerr << "opening file failed" << std::endl;
	if ( _configFile.bad() )
		std::cerr << "opened file is bad" << std::endl;
	if ( _configFile.eof() )
		std::cerr << "end of file while opening" << std::endl;
    tokenizer();
    _it = _tokens.begin();
    return;
}

webserv::parentConfig::~parentConfig( void ) {
    _configFile.close();
    return;
}

void webserv::parentConfig::tokenizer( void ){
	std::string line;

	while ( getline( _configFile, line ) ){
		for( int i = 0; line[i]; i++ ){
			if ( !isspace( line[i] ) )
				i += _newToken( &line[i] );
		}
	}
}

int webserv::parentConfig::_newToken( std::string line ){
	std::string token;
	int i = 0;

	if ( line[i] == ';' || line[i] == '{' || line[i] == '}' )
		token.append( &line[i++], 1 );
	else
		 do {
			if ( isspace( line[i] ) || line[i] == ';' || line[i] == '{' || line[i] == '}' )
				break;
			token.append( &line[i], 1 );
		} while ( line[++i] );
	_tokens.push_back( token );
	return --i;
}

int webserv::parentConfig::parseIntoPieces( socketData* socketData, httpData* httpData ){
	TokenType::iterator it = _tokens.begin();
	int ret;

	if ( (_it++)->compare("server") || *_it !="{" )
		return ERROR;
	while ( ++_it != _tokens.end() && *_it != "}" ) {
		if ( *_it == "listen" )
			ret = setSocket( socketData );
        else if ( *_it == "worker_connections" )
            ret = setWorkerConnections( socketData );
		else if ( *_it == "index" )
			ret = setIndex( httpData );
		else if ( *_it == "location" )
			ret = setLocation( httpData );
		else if ( *_it == "server_name" )
			ret = setServerName( httpData );
		else if ( *_it == "error_page" )
			ret = setErrorPage( httpData );
		else if ( *_it == "return" )
			ret = setRedirect( httpData );
		else if ( *_it == "}" )
			return SUCCES;
		if ( ret == ERROR )
			return ret;
	}
	if ( *(_it++) == "}" && _it != _tokens.end() )
		return NEOF;
	return SUCCES;
}

int webserv::parentConfig::setSocket( socketData* socketData ){
	try {
		socketData->addPort( stoi(*(++_it) ) );
	} catch ( std::exception &e ){ // TODO:: test to see if "iterator out of bounds" will be catched properly;
		std::cerr << "parentConfig::setSocket " << *_it << " " << e.what() << std::endl;
		return ERROR;
	}
	if ( _it == _tokens.end() || *(++_it) != ";" )
		return ERROR;
	for (; _it != _tokens.end() && *(_it + 1) == ";"; _it++ ){}
	return SUCCES;
}

int webserv::parentConfig::setWorkerConnections( socketData* socketData ){
    try {
        socketData->worker_connections = stoi(*(++_it) );
    } catch ( std::exception &e ){
        std::cerr << "parentConfig::setWorkerConnections " << *_it << " " << e.what() << std::endl;
        return ERROR;
    }
    if ( _it == _tokens.end() || *(++_it) != ";" )
        return ERROR;
    for (; _it != _tokens.end() && *(_it + 1) == ";"; _it++ ){}
    return SUCCES;
}

int webserv::parentConfig::setIndex( httpData* httpData ){ // TODO:: iterating untill ";" will give false positive in case of no ";" in file
	if (  _isWrongInput( NULL ) )
		return ERROR;
	for (; _it != _tokens.end() && *_it != ";"; _it++ )
		httpData->index.push_back( *_it );
	if ( _it == _tokens.end() || *(_it) != ";" )
		return ERROR;
	for (; _it != _tokens.end() && *(_it + 1) == ";"; _it++ ){}
	return SUCCES;
}

int webserv::parentConfig::setServerName( httpData* httpData ){ // TODO:: iterating untill ";" will give false positive in case of no ";" in file
	if (  _isWrongInput( NULL ) )
		return ERROR;
	for (; _it != _tokens.end() && *_it != ";"; _it++ )
		httpData->server_name.push_back( *_it );
	if ( _it == _tokens.end() || *(_it) != ";" )
		return ERROR;
	for (; _it != _tokens.end() && *(_it + 1) == ";"; _it++ ){}
	return SUCCES;
}

int webserv::parentConfig::setRedirect( httpData* httpData ){
	if (  _isWrongInput( NULL ) )
		return ERROR;
	try {
		httpData->redirect.emplace_back( std::make_pair(std::stoi( *(_it++) ), (*_it)));
	} catch ( std::exception &e ){
		std::cerr << "parentConfig::setRedirect " << *_it << " " << e.what() << std::endl; // TODO:: check if catch catches out of bound etc...
		return ERROR;
	}
	if ( _it == _tokens.end() || *(++_it) != ";" ) // TODO:: check different url's to see if some charachters mess up the tokenizer
		return ERROR;
	for (; _it != _tokens.end() && *(_it + 1) == ";"; _it++ ){}
	return SUCCES;
}

int webserv::parentConfig::setErrorPage( httpData* httpData ){
	if (  _isWrongInput( NULL ) )
		return ERROR;
	for (; _it != _tokens.end() && *_it != ";"; _it++ ){
		try {
			httpData->error_page.emplace_back( std::make_pair(std::stoi( *(_it++) ), (*_it)));
		} catch ( std::exception &e ){
			std::cerr << "parentConfig::setErrorPage " << *_it << " " << e.what() << std::endl;
			return ERROR;
		}
	}
	for (; _it != _tokens.end() && *(_it + 1) == ";"; _it++ ){}
	return SUCCES;
}

int webserv::parentConfig::setLocation( httpData* httpData ){
	int ret = SUCCES;
	webserv::locationData element;
	ret = _setLocation( element );
	if ( ret == SUCCES && *_it =="{") {
		while ( ++_it != _tokens.end() && *_it != "}"  ){
			if ( *_it == "root" )
				ret = _setRoot( element, httpData->abs_path );
//			else if ( *_it == "add_header" )
//				ret = _setAllowedResponse( element );
			else if ( *_it == "autoindex" )
				ret = _setAutoindex( element );
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

webserv::FileType& webserv::parentConfig::getFile( void ){
	return _configFile;
}

webserv::TokenType webserv::parentConfig::getTokens( void ){
	return _tokens;
}

int webserv::parentConfig::_setLocation( locationData& element ){
	if ( _isWrongInput( "{" ) )
		return ERROR;
	element.location = *_it++;
	return SUCCES;
}

int webserv::parentConfig::_setRoot( locationData& element, std::string abs_path ){
	if ( _isWrongInput( ";" ) )
		return ERROR;
	if ( element.root != "NONE" )
		return ERROR;
	element.root = abs_path;
	element.root.append( *_it );
	for (; _it != _tokens.end() && *(_it + 1) == ";"; _it++ ){}
	return SUCCES;
}

int webserv::parentConfig::_setAllowedResponse( locationData& element ){
	if ( _isWrongInput( ";" ) )
		return ERROR;

	// TODO:: figure right wy to format this
//	if ( *_it != "GET" ) )
//		element.allowed_response[GET] = true;

	return SUCCES;
}

int webserv::parentConfig::_setAutoindex( locationData& element ){
	if ( _isWrongInput( ";" ) )
		return ERROR;
	if ( *_it == "on" )
		element.autoindex = true;
	else if ( *_it == "off" )
		element.autoindex = false;
	else
		return ERROR;
	for (; _it != _tokens.end() && *(_it + 1) == ";"; _it++ ){}
	return SUCCES;
}

bool webserv::parentConfig::_isWrongInput( char* str ){
	if ( ++_it == _tokens.end() )
		return true;
	if ( *_it == "{"  )
		return true;
	if ( *_it == "}" )
		return true;
	if ( *_it == ";" )
		return true;
	if ( str && *(_it + 1 ) != str )
		return true;
	return false;
}



