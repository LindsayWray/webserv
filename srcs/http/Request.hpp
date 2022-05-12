#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include <fstream>
# include <sys/socket.h>
# include <sstream>

// ******* headers *******  //
#define TRANSFER_ENCODING "transfer-encoding"
#define CONTENT_LENGTH "content-length"
#define HOST "host"

namespace webserv {

    class Request {
    public:
        enum method {
            GET, POST, DELETE
        };
    private:
		std::string _rawRequest;
		bool _headersDone;
		long _contentLength;
		bool _chunked;
		bool _chunkedComplete;
        bool _chunkEndsWithHex;
        bool _chunkEndsWithSeparatedCRLF;
		long _remainder;
		long _maxClientBody;

        method _method;
        std::vector<std::string> _path;
        std::string _requestPath;
        std::string _version;
        std::map<std::string, std::string> _headers;
        std::string _body;
        std::string _host;

        void parse_statusline( std::string &method );
		void setPath( std::string line );
		void appendBody(const char* chunk, long len);
		void decodePath();


    public:
		Request(){};
		Request(int);

		Request(const Request& original);
		Request& operator=(const Request& original);

        std::string getBody() const;
        std::vector<std::string> getPath() const;
        method getMethod() const;
        std::string getRequestPath() const;

        void setRequestPath( std::string line );

        std::string getRawRequest() const;

        std::string getHost() const;

		void	parseChunk(char* chunk, int len);
		bool	isComplete() const;


        class MethodNotAllowedException : public std::exception {
        public:
            const char *what() const throw() {
                return "Method Not Allowed";
            }
        };
        class IncorrectRequestException : public std::exception {
        public:
            const char *what() const throw() {
                return "Request Not Valid";
            }
        };
        class NotImplementedException : public std::exception {
        public:
            const char *what() const throw() {
                return "Method Not Implemented";
            }
        };
        class VersionNotSupportedException : public std::exception {
        public:
            const char *what() const throw() {
                return "HTTP Version Not Implemented";
            }
        };
        class URITooLongException : public std::exception {
        public:
            const char *what() const throw() {
                return "URI Too Long";
            }
        };
		class MaxClientBodyException : public std::exception {
        public:
            const char *what() const throw() {
                return "Max Client Body Exceeded";
            }
        };
    };
}
#endif
