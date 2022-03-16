//
// Created by Kester kas De rooij on 3/16/22.
//

#include "parentConfig.hpp"


webserv::parentConfig::parentConfig( char* config_file ) {
    _configFile.open( config_file );
    return;
}

webserv::parentConfig::~parentConfig( void ) {
    _configFile.close();
    return;
}

int webserv::parentConfig::readParseConfigFile( socketData* t_socketData, httpData* t_httpData ){
    int server = 0;

    while ( getline( _configFile, _line ) )
        if ( _line.find( "server" ) )
            server = parseServer(t_socketData, t_httpData);
    if ( !server )
        server = parseServer(t_socketData, t_httpData);
    return server;
}

std::size_t webserv::parentConfig::checkLine( const char* str, char c ){
    std::size_t found = _line.find_first_not_of( str );

    if ( found == std::string::npos )
        return NEXTLINE;
    if ( str[found] != c )
        return ERROR;
    return found;
}

int webserv::parentConfig::parseServer( socketData *t_socketData, httpData *t_httpData ) {
    std::size_t check = checkLine( "server \n\t\v\f\r", '{');
    int bracket = 0;

    if ( check == ERROR )
        return ERROR;
    else if ( check == NEXTLINE ) {
        while ( getline( _configFile, _line ) ) {
            check = checkLine( " \n\t\v\f\r", '{');
            if ( check == ERROR )
                return ERROR;
            if ( check != NEXTLINE )
                break;
        }
    }
    while ( getline( _configFile, _line ) ) {
        if ( _line.find("index") != std::string::npos )

    }
}
//
//void webserv::parentConfig::parseLine( std::string line ){
//    static int type = NONE;
//
//    if( line.find("index") || type == INDEX ) {
//
//        parseIndex(line, &type);
//    }
//    if( )
//}
