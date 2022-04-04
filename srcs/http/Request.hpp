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
		enum method {GET, POST, DELETE};
	private:
		method			_method;
		std::vector<std::string>		_path;
		std::string 	_requestPath;
		std::string		_version;
		std::map<std::string, std::string>	_headers;
		std::string _body;

		void	parse_statusline(std::string& method);

	public:
		Request(std::string);
		
		std::string getBody() const;
		std::vector<std::string> getPath() const;
		method getMethod() const;
		std::string getRequestPath() const;
		void setPath( std::string line );

		class IncorrectRequestException : public std::exception{
		public:
			const char* what() const throw(){
				return "Request Not Valid";
			}
		};
	};
}
#endif