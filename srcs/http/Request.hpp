#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include <fstream>
# include <sys/socket.h>
# include <sstream> 


// POST /cgi-bin/process.cgi HTTP/1.1
// User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
// Host: www.tutorialspoint.com
// Content-Type: application/x-www-form-urlencoded
// Content-Length: length

// licenseID=string&content=string&/paramsXML=string
// xml version="1.0" encoding="utf-8"?>
// string xmlns="ht//clearforest.com/">string/string

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