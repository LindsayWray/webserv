#include "config/parentConfig.hpp"
#include "utils/stringUtils.hpp"

int main( int argc, char **argv, char **envp ) {
    if ( argc != 2 )
        return EXIT_FAILURE;
	webserv::httpData http;
	webserv::parentConfig object( webserv::setFileLocation( http, envp, "/var/sites_enabled/", argv[1] ) );
	webserv::socketData socket;

	if ( object.parseIntoPieces( &socket, &http ) == ERROR )
		return EXIT_FAILURE;
		
	webserv::testServer server( socket, http );
	server.launch();
	return EXIT_SUCCESS;
}
