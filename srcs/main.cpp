//#include "server/testServer.hpp"
#include "config/parentConfig.hpp"
//#include "utils/dataStructs.hpp"
#include "utils/stringUtils.hpp"

int main( int argc, char **argv, char **envp ) {
	std::string configFile = webserv::setFileLocation( envp, "/config/config.webserv" );
	std::cout << configFile << std::endl;
	webserv::parentConfig object( configFile );
	std::vector<std::string> thing = object.getTokens();
	webserv::httpData http;
	webserv::socketData socket;

	if ( object.parseIntoPieces( &socket, &http ) == ERROR )
		return EXIT_FAILURE;
		
	webserv::testServer server( socket, http );
	server.launch();
	return EXIT_SUCCESS;
}
