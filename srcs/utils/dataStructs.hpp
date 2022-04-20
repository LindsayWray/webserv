#ifndef WEBSERV_DATASTRUCTS_HPP
#define WEBSERV_DATASTRUCTS_HPP

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <netinet/in.h>
#include <algorithm>
#include "../http/Request.hpp"
#include "../sockets/listeningSocket.hpp"

#define COMPLETE_MAP std::map<int, std::pair<webserv::listeningSocket*,webserv::httpData> >
#define REDIRECTION -3
#define NOTFOUND -2
#define ERROR -1
#define SUCCES 0
#define NEOF 1
#define CGI_SIGN 2

namespace webserv {

    struct locationData {
        std::vector<std::string> path;
        std::string root;
        std::vector<std::string> cgi_param;
        std::pair<int, std::string> redirect;
        std::string index;
        bool allowed_response[3];
        bool autoindex;
        bool CGI;

        locationData( std::string _root );

        int pathTokenizer( std::string line );

        int cgiTokenizer( std::string line );
    };

    class httpData {
    public:
        char** env;
        std::string absPath;
        std::vector<std::string> serverName;
        std::map<int, std::string> errorPage;
        std::vector<locationData> locations;
        int maxClientBody; // in kb
        int port;
        std::set<std::string> created_files;

        httpData( void );

        httpData( std::string root );

        httpData( const httpData& original );

        httpData& operator=( const httpData& original );

        ~httpData();

        void organizeLocations( void );

        std::string formatErrorPage( std::string message );

        void setErrorPages( void );

    };

    struct kqConData {
        int kq;
        int nbrConnections;
        int workerConnections;
    };

    typedef struct cgi_response {
        int clientFd;
        int pid;
        std::string body;
    } cgi_response;

    typedef struct serverData {
        webserv::kqConData kqData;

        COMPLETE_MAP serverMap; //key = fd
        std::map<std::pair<int, std::string>, httpData> hostServername; //key = port & servername
        std::map<int, httpData> defaultServer; //key = port

        std::map<int, webserv::Request> requests;
        std::map<int, std::pair<std::string, long> > responses;
        std::map<int, httpData> clientSockets;
        std::map<int, cgi_response> cgiResponses;

        char* buf;
        int buflen;
        bool closeConnection;
    } serverData;
}


#endif //WEBSERV_DATASTRUCTS_HPP
