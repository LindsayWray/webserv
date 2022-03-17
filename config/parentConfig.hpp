//
// Created by Kester kas De rooij on 3/16/22.
//

#ifndef WEBSERV_PARENTCONFIG_HPP
#define WEBSERV_PARENTCONFIG_HPP

#include "../utils/dataStructs.hpp"
#include "../server/testServer.hpp"
#include <fstream>
#include <vector>

#define ERROR -1
#define NEXTLINE 0

#define NONE 0
#define INDEX 1

namespace webserv{

    class parentConfig{
    private:
        std::ifstream _configFile;
        std::string _line;
        std::vector<std::string> _tokens;

    public:
        parentConfig( const char* config_file );
        ~parentConfig();

        int readParseConfigFile( socketData* t_socketData, httpData* t_httpData );
        void tokenizer( void );
        int newToken( std::string line );
        std::vector<std::string> getTokens( void );
        int parseServer( socketData* t_socketData, httpData* t_httpData );
        std::size_t checkLine( const char* str, char c );
//        void parseLine( std::string line );

    };
}

#endif //WEBSERV_PARENTCONFIG_HPP
