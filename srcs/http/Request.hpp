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
		std::string _rawRequest;
		bool _headersDone;
		int _contentLength;
		int _max_client_body;

        method _method;
        std::vector<std::string> _path;
        std::string _requestPath;
        std::string _version;
        std::map<std::string, std::string> _headers;
        std::string _body;

        void parse_statusline( std::string &method );
		void setPath( std::string line );


    public:
		Request(){};
		//Request(int);

		Request& operator=(const Request& original);

        std::string getBody() const;
        std::vector<std::string> getPath() const;
        method getMethod() const;
        std::string getRequestPath() const;
		std::string getRawRequest() const;

		void	parseChunk(char* chunk, int len);
		bool	isComplete() const;

        class IncorrectRequestException : public std::exception {
        public:
            const char *what() const throw() {
                return "Request Not Valid";
            }
        };
    };
}
#endif