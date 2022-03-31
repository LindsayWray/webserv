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
	//status line
		method			_method;
		std::string		_path;
		std::string		_version;
	//headers
		std::map<std::string, std::string>	_headers;

	//body
		std::string _body;

		void	parse_statusline(std::string& method);

	public:
		Request(std::string);
		
		std::string getBody() const;
		std::string getPath() const;
		method getMethod() const;

		class IncorrectRequestException : public std::exception{
		public:
			const char* what() const throw(){
				return "Request Not Valid";
			}
		};
	};
}
#endif