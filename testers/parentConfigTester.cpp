//
// Created by Kas on 17/03/2022.
//

#include "../config/parentConfig.hpp"

int main( void ){
	webserv::parentConfig object( "../config/config.webserv" );
	std::vector<std::string> thing = object.getTokens();

	for ( std::vector<std::string>::iterator it = object.getTokens().begin(); it < object.getTokens().end(); it++)
		std::cout << *it << std::endl;
}