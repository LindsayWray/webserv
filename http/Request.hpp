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
	private:
	//status line
		std::string		_method;
		std::string		_path;
		std::string		_version;
	//headers
		std::map<std::string, std::string>	_headers;

	//body
		std::string _body;

		void	parse_statusline();

	public:
		Request(std::string);

		class IncorrectRequestException : public std::exception{
		public:
			const char* what() const throw(){
				return "Request Not Valid";
			}
		};
	};
}
#endif