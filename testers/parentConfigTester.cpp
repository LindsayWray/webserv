//
// Created by Kas on 17/03/2022.
//

#include "../config/parentConfig.hpp"
#include "../utils/stringUtils.hpp"

int main( int arc, char** argv, char** env ){
	std::string configFile = webserv::setFileLocation( env, "/config/config.webserv" );
	webserv::parentConfig object( configFile );
	std::vector<std::string> thing = object.getTokens();
	webserv::httpData http;
	webserv::socketData socket;

	std::cout << object.parseIntoPieces( &socket, &http ) << std::endl;
	for ( int i = 0; i < socket.ports.size(); i++)
		std::cout << "port " << socket.ports[i] << std::endl;
	for ( std::vector<std::string>::iterator it = http.server_name.begin(); it != http.server_name.end(); it++)
		std::cout << "server_name " << *it << std::endl;
	for ( std::vector<std::string>::iterator it = http.index.begin(); it != http.index.end(); it++)
		std::cout << "index " << *it << std::endl;
	for ( std::vector<std::pair<int,std::string>>::iterator it = http.error_page.begin(); it != http.error_page.end(); it++)
		std::cout << "error_page " << it->first << " " << it->second << std::endl;
	for ( std::vector<std::pair<int,std::string>>::iterator it = http.redirect.begin(); it != http.redirect.end(); it++)
		std::cout << "redirect " << it->first << " " << it->second << std::endl;
}