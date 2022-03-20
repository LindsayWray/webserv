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
        TokenType::iterator _it;
        TokenType _tokens;

    public:
        parentConfig( std::string config_file );
        ~parentConfig();

        void tokenizer( void );
        int parseIntoPieces( socketData* socketData, httpData* httpData );

        int setSocket( socketData* socketData );
        int setIndex( httpData* httpData );
        int setLocation( httpData* httpData );
        int setServerName( httpData* httpData );
        int setErrorPage( httpData* httpData );
        int setRedirect( httpData* httpData );

        FileType& getFile( void );
        TokenType getTokens( void );

    private:
        int _newToken( std::string line );
        int _setLocation( locationData& element );
        int _setRoot( locationData& element );
        int _setAllowedResponse( locationData& element );
        int _setAutoindex( locationData& element );
    };
}

#endif //WEBSERV_PARENTCONFIG_HPP
