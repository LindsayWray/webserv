#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include <fstream>
# include <sys/socket.h>
# include <sstream>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

namespace webserv {

	class Request {
	public:
		enum method {GET, POST, DELETE};
	private:
	//status line
		method			_method;
		std::vector<std::string>		_path;
		std::string 	_requestPath;
		std::string		_version;
	//headers
		std::map<std::string, std::string>	_headers;

	//body
		std::string _body;

		void	parse_statusline(std::string& method);

	public:
		Request(std::string);
		
		std::string getBody() const;
		std::vector<std::string> getPath() const;
		method getMethod() const;
		std::string getRequestPath() const;

		class IncorrectRequestException : public std::exception{
		public:
			const char* what() const throw(){
				return "Request Not Valid";
			}
		};

		void setPath( std::string line ){
            std::size_t i = 0, found;

            if ( line[i] != '/' )
                return;
            while( i < line.length() ){
                found = line.find_first_of( "/", i);
                if ( found == std::string::npos ) {
                    _path.push_back( line.substr( i, line.length() ) );
                    break;
                }else {
                    if ( i != found - i )
                        _path.push_back( line.substr( i, found - i ) );
                    _path.push_back( line.substr( found, 1 ) );
                    i = found + 1;
                }
            }
            std::cout << YELLOW << "printing incoming vec" << std::endl;
            for ( int i = 0; i < _path.size(); i++)
            	std::cout << GREEN << _path[i] << RESET << std::endl;
		}
    };

}
#endif