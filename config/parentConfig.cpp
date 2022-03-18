//
// Created by Kester kas De rooij on 3/16/22.
//

#include "parentConfig.hpp"

webserv::parentConfig::parentConfig( const char* config_file ) {
	_configFile.open( config_file, std::fstream::in );
	if ( _configFile.fail() )
		std::cerr << "opening file failed" << std::endl;
	if ( _configFile.bad() )
		std::cerr << "opened file is bad" << std::endl;
	if ( _configFile.eof() )
		std::cerr << "end of file while opening" << std::endl;
    tokenizer();
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

int webserv::parentConfig::parseIntoPieces( httpData* httpData, socketData* socketData ){
	TokenType::iterator it = _tokens.begin();
	int ret;

	if ( (it++)->compare("server") || it->compare("{") )
		return ERROR;
	while ( ++it != _tokens.end() && it->compare( "}" ) ) {
		if ( it->compare( "listen" ) )
			ret = setSocket( &it, socketData );
		else if ( it->compare( "index" ) )
			ret = setIndex( &it, httpData );
		else if ( it->compare( "location" ) )
			ret = setLocation( &it, httpData );
		else if ( it->compare( "server_name" ) )
			ret = setServerName( &it, httpData );
		else if ( it->compare( "error_page" ) )
			ret = setErrorPage( &it, httpData );
		else if ( it->compare( "return" ) )
			ret = setRedirect( &it, httpData );
		else
			return ERROR;
		if ( ret == ERROR )
			return ret;
	}
	return SUCCES;
}

int webserv::parentConfig::setSocket( TokenType::iterator* it, socketData* socketData ){
	TokenType::iterator tmp = ++(*it);

	try {
		socketData->addPort( stoi(*tmp) );
	} catch ( std::exception &e ){ // TODO:: test to see if "iterator out of bounds" will be catched properly;
		std::cerr << "parentConfig::setSocket " << e.what() << std::endl;
		return ERROR;
	}
	if ( (++tmp)->compare(";") )
		return ERROR;
	*it = tmp;
	return SUCCES;
}

int webserv::parentConfig::setIndex( TokenType::iterator* it, httpData* httpData ){
	TokenType::iterator tmp = ++(*it);

	if ( tmp == _tokens.end() || !(++tmp)->compare(";") )
		return ERROR;
	for (; tmp != _tokens.end() && (tmp)->compare(";"); tmp++ )
		httpData->index.push_back( *tmp );
	if ( tmp == _tokens.end() )
		return ERROR;
	*it = tmp;
	return SUCCES;
}

int webserv::parentConfig::setServerName( TokenType::iterator* it, httpData* httpData ){
	TokenType::iterator tmp = ++(*it);

	if ( tmp == _tokens.end() || !(++tmp)->compare(";") )
		return ERROR;
	for (; tmp != _tokens.end() && (tmp)->compare(";"); tmp++ )
		httpData->server_name.push_back( *tmp );
	if ( tmp == _tokens.end() )
		return ERROR;
	*it = tmp;
	return SUCCES;
}

int webserv::parentConfig::setRedirect( TokenType::iterator* it, httpData* httpData ){
	TokenType::iterator tmp = ++(*it);

	if ( tmp == _tokens.end() || !(++tmp)->compare(";") )
		return ERROR;
	try {
		httpData->redirect.emplace_back( std::make_pair(std::stoi( *(tmp++) ), (*tmp)));
	} catch ( std::exception &e ){
		std::cerr << "parentConfig::setRedirect " << e.what() << std::endl; // TODO:: check if catch catches out of bound etc...
		return ERROR;
	}
	if ( tmp == _tokens.end() || (++tmp)->compare(";") ) // TODO:: check different url's to see if some charachters mess up the tokenizer
		return ERROR;
	*it = tmp;
	return SUCCES;
}

int webserv::parentConfig::setErrorPage( TokenType::iterator* it, httpData* httpData ){
	TokenType::iterator tmp = ++(*it);

	if ( tmp == _tokens.end() || !(++tmp)->compare(";") )
		return ERROR;
	for (; tmp != _tokens.end() && (tmp)->compare(";"); tmp++ ){
		try {
			httpData->error_page.emplace_back( std::make_pair(std::stoi( *(tmp++) ), (*tmp)));
		} catch ( std::exception &e ){
			std::cerr << "parentConfig::setErrorPage " << e.what() << std::endl;
			return ERROR;
		}
	}
	*it = tmp;
	return SUCCES;
}

int webserv::parentConfig::setLocation( TokenType::iterator* it, httpData* httpData ){
	TokenType::iterator tmp = ++(*it);
	int ret = SUCCES;
//
//	if ( tmp == _tokens.end() || (++tmp) == ";" )
//		return ERROR;
//	if ( !(++tmp)->compare("{") || (++tmp)->compare(";") )
	*it = tmp;
	return ret;
}

webserv::FileType& webserv::parentConfig::getFile( void ){
	return _configFile;
}

webserv::TokenType webserv::parentConfig::getTokens( void ){
	return _tokens;
}


