//
// Created by Kas on 17/03/2022.
//

#include "../config/parentConfig.hpp"

int main( void ){
	webserv::parentConfig object( "../config/config.webserv" );
	std::vector<std::string> thing = object.getTokens();
	webserv::httpData http;
	webserv::socketData socket;

	for ( std::vector<std::string>::iterator it = object.getTokens().begin(); it < object.getTokens().end(); it++)
		std::cout << *it << std::endl;
	std::cout << object.parseIntoPieces( &http, &socket) << std::endl;
	for ( int i = 0; i < (sizeof(socket.ports) / sizeof(socket.ports[0] )); i++)
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