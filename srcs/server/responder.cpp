#include "server.hpp"
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>

void responder(int fd, HTTPResponseMessage response) {
	/* TEST WITH RESPONSE MESSAGE READ FROM FILE */
		// std::ifstream outfile;
		// std::string line;

		// std::cout << "sending response" << std::endl;

		// outfile.open("var/www/html/test.html");
		// while( std::getline( outfile, line ) ) {
		// 	send( fd, line.c_str(), line.length(), 0 );
		// 	send( fd, "\n", 1, 0 );
		// }
		// send( fd, "\n", 1, 0 );
		// send( fd, "\n", 1, 0 );

	/* RESPONSE LOGGER */
		// std::cout << response.toString();

	/* FIRST ATTEMPT AT RESPONSE IMPL */
		std::cout << "sending response" << std::endl;

		const std::string& responseStr = response.toString();

		//std::cout << "responseStr " << responseStr << std::endl;
		send( fd, responseStr.c_str(), responseStr.length(), 0 );

	/* TEST WITH CONSTRUCTED RESPONSE */
		// std::ifstream outfile;
		// std::string line;
		// std::string body;
		// outfile.open("var/www/html/test.html");
		// while( std::getline( outfile, line ) ) {
		// 	body += line;
		// }
		// HTTPResponseMessage sut;
		// sut.addStatus(HTTPResponseMessage::NOT_FOUND)
		// 	.addType(HTTPResponseMessage::contentTypes.at("html"))
		// 	.addLength(body.length())
		// 	.addBody(body);
		// std::cout << sut.toString();
		// send( fd, sut.toString().c_str(), sut.toString().length(), 0);
}