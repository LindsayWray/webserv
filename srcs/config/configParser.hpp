//
// Created by Kester kas De rooij on 3/16/22.
//

#ifndef WEBSERV_CONFIGPARSER_HPP
#define WEBSERV_CONFIGPARSER_HPP

#include "../utils/dataStructs.hpp"
#include <fstream>
#include <vector>

namespace webserv {

    typedef std::vector<std::string> TokenType;
    typedef std::ifstream FileType;

    class configParser {

    private:
        FileType _configFile;
        TokenType _tokens;
        TokenType::iterator _it;

    public:
        configParser( std::string config_file );

        ~configParser();

        void tokenizer( void );

        int parseIntoPieces( socketData *socketData, httpData *httpData );

        int setSocket( socketData *socketData );

        int setIndex( httpData *httpData );

        int setLocation( httpData *httpData );

        int setServerName( httpData *httpData );

        int setErrorPage( httpData *httpData );

        int setRedirect( httpData *httpData );

        int setWorkerConnections( socketData *socketData );

        FileType &getFile( void );

        TokenType getTokens( void );

    private:
        int _newToken( std::string line );

        int _setLocation( locationData &element );

        int _setRoot( locationData &element );

        int _setCgiParam( locationData &element );

        int _setAllowedResponse( locationData &element );

        int _setAutoindex( locationData &element );

        bool _isWrongInput( char * );
    };
}

#endif //WEBSERV_CONFIGPARSER_HPP
