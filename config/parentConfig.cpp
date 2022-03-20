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

	if ( (_it++)->compare("server") || _it->compare("{") )
		return ERROR;
	while ( ++_it != _tokens.end() && _it->compare( "}" ) ) {
		if ( !_it->compare( "listen" ) )
			ret = setSocket( socketData );
		else if ( !_it->compare( "index" ) )
			ret = setIndex( httpData );
		else if ( !_it->compare( "location" ) )
			ret = setLocation( httpData );
		else if ( !_it->compare( "server_name" ) )
			ret = setServerName( httpData );
		else if ( !_it->compare( "error_page" ) )
			ret = setErrorPage( httpData );
		else if ( !_it->compare( "return" ) )
			ret = setRedirect( httpData );
		else if ( !_it->compare("}") )
			return SUCCES;
		if ( ret == ERROR )
			return ret;
	}
	return SUCCES;
}

int webserv::parentConfig::setSocket( socketData* socketData ){
	try {
		socketData->addPort( stoi(*(++_it) ) );
	} catch ( std::exception &e ){ // TODO:: test to see if "iterator out of bounds" will be catched properly;
		std::cerr << "parentConfig::setSocket " << *_it << " " << e.what() << std::endl;
		return ERROR;
	}
	if ( (++_it)->compare(";") )
		return ERROR;
	return SUCCES;
}

int webserv::parentConfig::setIndex( httpData* httpData ){
	if ( ++_it == _tokens.end() || !(_it)->compare(";") || !(_it + 1)->compare(";") )
		return ERROR;
	for (; _it != _tokens.end() && (_it)->compare(";"); _it++ )
		httpData->index.push_back( *_it );
	if ( _it == _tokens.end() )
		return ERROR;
	return SUCCES;
}

int webserv::parentConfig::setServerName( httpData* httpData ){
	if ( ++_it == _tokens.end() || !(_it)->compare(";") || !(_it + 1)->compare(";") )
		return ERROR;
	for (; _it != _tokens.end() && (_it)->compare(";"); _it++ )
		httpData->server_name.push_back( *_it );
	if ( _it == _tokens.end() )
		return ERROR;
	return SUCCES;
}

int webserv::parentConfig::setRedirect( httpData* httpData ){
	if ( ++_it == _tokens.end() || !(_it)->compare(";") || !(_it + 1)->compare(";") )
		return ERROR;
	try {
		httpData->redirect.emplace_back( std::make_pair(std::stoi( *(_it++) ), (*_it)));
	} catch ( std::exception &e ){
		std::cerr << "parentConfig::setRedirect " << *_it << " " << e.what() << std::endl; // TODO:: check if catch catches out of bound etc...
		return ERROR;
	}
	if ( _it == _tokens.end() || (++_it)->compare(";") ) // TODO:: check different url's to see if some charachters mess up the tokenizer
		return ERROR;
	return SUCCES;
}

int webserv::parentConfig::setErrorPage( httpData* httpData ){
	if ( ++_it == _tokens.end() || !(_it)->compare(";") || !(_it + 1)->compare(";") )
		return ERROR;
	for (; _it != _tokens.end() && (_it)->compare(";"); _it++ ){
		try {
			httpData->error_page.emplace_back( std::make_pair(std::stoi( *(_it++) ), (*_it)));
		} catch ( std::exception &e ){
			std::cerr << "parentConfig::setErrorPage " << *_it << " " << e.what() << std::endl;
			return ERROR;
		}
	}
	return SUCCES;
}

int webserv::parentConfig::setLocation( httpData* httpData ){
	int ret = SUCCES;
	webserv::locationData element;
	if ( ++_it == _tokens.end() )
		return ERROR;
	ret = _setLocation( element );
	if ( ret == SUCCES && !_it->compare("{")) {
		while ( ++_it != _tokens.end() && _it->compare( "}" ) ){
			if ( !_it->compare( "root" ))
				ret = _setRoot( element );
			else if ( !_it->compare( "add_header" ))
				ret = _setAllowedResponse( element );
			else if ( !_it->compare( "autoindex" ))
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
	if ( _it == _tokens.end() || !_it->compare( "{" ) || !_it->compare( "}" ) || !_it->compare( ";" ) )
		return ERROR;
	element.location = *_it++;
	return SUCCES;
}

int webserv::parentConfig::_setRoot( locationData& element ){
	if ( ++_it == _tokens.end() || !_it->compare( "{" ) || !_it->compare( "}" ) || !_it->compare( ";" ) )
		return ERROR;
	if ( element.root.compare( "NONE" ) )
		return ERROR;
	element.root = *_it++;
	return SUCCES;
}

int webserv::parentConfig::_setAllowedResponse( locationData& element ){
	if ( ++_it == _tokens.end() || !_it->compare( "{" ) || !_it->compare( "}" ) || !_it->compare( ";" ) )
		return ERROR;

	// TODO:: figure right wy to format this

	return SUCCES;
}

int webserv::parentConfig::_setAutoindex( locationData& element ){
	if ( ++_it == _tokens.end() || !_it->compare( "{" ) || !_it->compare( "}" ) || !_it->compare( ";" ) )
		return ERROR;
	if ( !_it->compare( "on" ) )
		element.autoindex = true;
	else if ( !_it->compare( "off" ) )
		element.autoindex = false;
	else
		return ERROR;
	_it++;
	return SUCCES;
}

