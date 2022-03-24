//
// Created by Kester kas De rooij on 3/14/21.
//

#include "catch.hpp"
#include "configParser.hpp"


TEST_CASE( "configParsing", "[configParser]" ){
	webserv::configParser object("config.webserv" );
	std::vector<std::string> thing = object.getTokens();
    SECTION( "index" ){
		REQUIRE( thing[0] == "server{" );
        REQUIRE( thing[1] == "listen" );
	}
}
