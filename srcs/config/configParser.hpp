#ifndef WEBSERV_CONFIGPARSER_HPP
#define WEBSERV_CONFIGPARSER_HPP

#include "../utils/dataStructs.hpp"
#include <fstream>
#include <vector>

namespace webserv {

    typedef std::vector<std::string> TokenType;
    typedef std::ifstream FileType;

    class configParser {

        enum e_configErrorCodes {
            NONE,
            NOFILE,
            BADFILE,
            EMPTYFILE,
            SERVER,
            SOCKET,
            WORKERC,
            INDEX,
            MAXBODY,
            LOCATION,
            SERVERNAME,
            ERRORPAGE,
            REDIRECT,
            ROOT,
            CGIPARAM,
            LIMITEDMETHOD,
            AUTOINDEX
        };

    private:
        FileType _configFile;
        TokenType _tokens;
        TokenType::iterator _it;
        int _errorCode;

    public:
// ******* configParser.cpp *******  //
        configParser( std::string config_file );

        ~configParser();

        void tokenizer( void );

        int parseIntoPieces( socketData &socketData, httpData &httpData );

// ******* configParserSetters.cpp *******  //
        int setSocket( socketData &socketData, httpData &httpData );

        int setClientMaxBodySize( webserv::httpData &httpData );

        int setServerName( httpData &httpData );

        int setErrorPage( httpData &httpData );

        int setRedirect( httpData &httpData );

// ******* configParserLocation.cpp *******  //
        int setLocation( httpData &httpData );

// ******* configParserUtils.cpp *******  //
        FileType &getFile( void );

        TokenType getTokens( void );

        int checkErrorCode( void );

    private:
// ******* configParser.cpp *******  //
        int _newToken( std::string line );

// ******* configParserLocation.cpp *******  //
        int _setLocation( locationData &element );

        int _setRoot( locationData &element );

        int _setIndex( locationData &element );

        int _setCgiParam( locationData &element );

        int _setLimitedMethod( locationData &element );

        int _setAutoindex( locationData &element );

// ******* configParserUtils.cpp *******  //
        bool _isWrongInput( char * );

        bool _isCorrectCode( int input );

        int _endOfLine( int errorcode );

    };
}

#endif //WEBSERV_CONFIGPARSER_HPP
