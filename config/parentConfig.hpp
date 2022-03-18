//
// Created by Kester kas De rooij on 3/16/22.
//

#ifndef WEBSERV_PARENTCONFIG_HPP
#define WEBSERV_PARENTCONFIG_HPP

#include "../utils/dataStructs.hpp"
#include "../server/testServer.hpp"
#include <fstream>
#include <vector>

namespace webserv{

	typedef std::vector<std::string> TokenType;
	typedef std::ifstream FileType;

    class parentConfig{

    private:
        FileType _configFile;
        std::string _line;
        TokenType _tokens;

    public:
        parentConfig( const char* config_file );
        ~parentConfig();

        void tokenizer( void );
        int parseIntoPieces( httpData* httpData, socketData* socketData );
        FileType& getFile( void );
        TokenType getTokens( void );

        int setSocket( TokenType::iterator* it, socketData* socketData );
        int setIndex( TokenType::iterator* it, httpData* httpData );
        int setLocation( TokenType::iterator* it, httpData* httpData );
        int setServerName( TokenType::iterator* it, httpData* httpData );
        int setErrorPage( TokenType::iterator* it, httpData* httpData );
        int setRedirect( TokenType::iterator* it, httpData* httpData );

    private:
        int _newToken( std::string line );

    };
}

#endif //WEBSERV_PARENTCONFIG_HPP
