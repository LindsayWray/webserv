//
// Created by Kester kas De rooij on 3/15/22.
//

#ifndef WEBSERV_DATASTRUCTS_HPP
#define WEBSERV_DATASTRUCTS_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <netinet/in.h>
#include <algorithm>

#define ERROR -1
#define SUCCES 0
#define NEOF 1

namespace webserv{

    struct socketData{
        int domain;
        int service;
        int protocol;
        std::vector<int> ports;
        u_long interface;
        int backlog;
        int worker_connections;

        socketData( void ){
            domain = AF_INET;
            service = SOCK_STREAM;
            protocol = 0;
            //ports.push_back( 80 );
            interface = INADDR_ANY;
            backlog = 32;
            worker_connections = 10;
        }
        void addPort( int newPort ){ // start on port 80, if no port 80 is defined use other
            if ( std::find( ports.begin(), ports.end(), newPort ) == ports.end() )
                ports.push_back( newPort );
        }
    };

    struct locationData {
        std::string location;
        std::string root;
        int allowed_response[3]; // GET = 0, POST = 1, DELETE = 2
        bool autoindex;
        locationData( void ){
            root = "NONE";
            memset( allowed_response, 1, 3);
            autoindex = false;
        }
    };

    class httpData{
    public:
        std::string abs_path;
        std::vector<std::string> server_name;
        std::vector<std::string> index;
        std::map<int, std::string> error_page; // maybe map because of unique key's
        std::vector<std::pair<int, std::string> > redirect; // but vector is chronological which is nice
        std::vector<locationData> locations;
        
        httpData( std::string root ) : abs_path( root ) {}
        ~httpData() {}

        /* EXAMPLE **
         * GET /images/2022/03/04/1400x1800.jpeg HTTP/1.1
         *  pathFromHTTPRequest = /images/2022/03/04/1400x1800.jpeg
         *  Returns the filepath to the appropriate file in the internal filesystem
         *  Returns [...]/AUTOINDEX.html or [...]/index.html if path leads to directory
         * 
         * if /images/ or /images is defined as a location inside
         * the appropriate server block in the config.webserv:
         *  reqPath = "/images/"
         *  reqPathInfo = "2022/03/04/1400x1800.jpeg"
         * else:
         *  reqPath = "/"
         *  reqPathInfo = "images/2022/03/04/1400x1800.jpeg"
         * 
         * getRequestedFilePath() then returns
         *  filePath = root + reqPathInfo
         *  where root is defined in the appropriate location block
         *      (locationData.root)
         *  If no root is defined inside the location block in the config.webserv,
         *  the parser should assign it to be:
         *      locationData.root = httpData.abs_path + reqPath;
         * 
         * ASSUMPTIONS:
         * - locationData.root and abs_path are both directory paths not ending with a slash
         * - everything with a '.' in the filepath is a file and not a directory
         */

        std::string getRequestedFilePath(std::string pathFromHTTPRequest) {
            std::string filePath;
            std::string root;
            std::string reqPath = _getReqPath(pathFromHTTPRequest);
            std::string reqPathInfo;

            locationData *location = _findLocationBlock(reqPath);
            if (location && reqPath != "/") // if /resources/
                root = location->root;
            else {                          // if / or /troep/
                location = _findLocationBlock("/");
                if (location)               // if '/' listed as location in config
                    root = location->root + reqPath;
                else                        // if '/' not listed as location in config
                    root = this->abs_path + reqPath;
            }

            if (reqPath.length() >= pathFromHTTPRequest.length())
                reqPathInfo = "";
            else
                reqPathInfo = pathFromHTTPRequest.substr(reqPath.length());

            bool pathIsDirectory = (reqPathInfo.length() == 0 || reqPathInfo.back() == '/');
            if (pathIsDirectory == false && reqPathInfo.find_last_of('.') == std::string::npos) {
                pathIsDirectory = true;
                reqPathInfo += '/';
            }
            
            filePath = root + reqPathInfo;

            if (pathIsDirectory && location && location->autoindex)
                filePath += "AUTOINDEX.HTML";
            else if (pathIsDirectory)
                filePath += "index.html";

            return filePath;
        }

    // Helper functions, unprivated for testing purposes
    // private:
        std::string _getReqPath(std::string pathFromHTTPRequest) {
            std::string reqPath;

            std::size_t firstSlash = pathFromHTTPRequest.find_first_of('/');
            std::size_t secondSlash = pathFromHTTPRequest.find_first_of('/', firstSlash + 1);

            bool pathContainsSingleSlash = (secondSlash == std::string::npos);
            if (pathContainsSingleSlash)    // /pathWithoutSecondSlashMustBeEitherFileOrDirectory(.jpg)
            {
                bool isFile = (pathFromHTTPRequest.find('.') != std::string::npos); 
                if (isFile)
                    reqPath = "/";
                else
                    reqPath = pathFromHTTPRequest + "/";
            }
            else                            // /path/contains/multiple/directories(.jpg) or /path/, either way /path/ is a directory for sure
                reqPath = pathFromHTTPRequest.substr(0, secondSlash - firstSlash + 1);
            
            if (reqPath.length() == 2 && reqPath.back() == '/')
                reqPath.pop_back();

            return reqPath;
        }

        locationData*   _findLocationBlock(std::string locationStr) {
            std::vector<locationData>::iterator it = this->locations.begin();
            for ( ; it != this->locations.end(); it++) {
                if (locationStr == it->location)
                    return &(*it);
            }
            return NULL;
        }
    };

   	struct readData{
        char* buf;
        int buflen;
        int bytesread;
    };

    struct kqConData {
        int kq;
        int nbr_connections;
        int worker_connections;
    };

};

#endif //WEBSERV_DATASTRUCTS_HPP
