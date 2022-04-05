#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include <fstream>
# include <sys/socket.h>
# include <sstream>

namespace webserv {

    class Request {
    public:
        enum method {
            GET, POST, DELETE
        };
    private:
        //status line
        method _method;
        std::vector<std::string> _path;
        std::string _requestPath;
        std::string _version;
        //headers
        std::map<std::string, std::string> _headers;

        //body
        std::string _body;

        void parse_statusline( std::string &method );

    public:
        Request( std::string );

        std::string getBody() const;

        std::vector<std::string> getPath() const;

        method getMethod() const;

        std::string getRequestPath() const;

        class IncorrectRequestException : public std::exception {
        public:
            const char *what() const throw() {
                return "Request Not Valid";
            }
        };

        void setPath( std::string line ) {
            std::size_t i = 0, found;

            if ( line[i] != '/' )
                return;
            while ( i < line.length()) {
                found = line.find_first_of( "/", i );
                if ( found == std::string::npos ) {
                    _path.push_back( line.substr( i, line.length()));
                    break;
                } else {
                    if ( i != found - i )
                        _path.push_back( line.substr( i, found - i ));
                    _path.push_back( line.substr( found, 1 ));
                    i = found + 1;
                }
            }
        }
    };
}
#endif