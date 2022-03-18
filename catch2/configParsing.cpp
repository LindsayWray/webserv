//
// Created by Kester kas De rooij on 3/14/21.
//

#include "catch.hpp"
#include "../config/parentConfig.hpp"


TEST_CASE( "configParsing", "[parentConfig]" ){
	webserv::parentConfig object( "config.webserv" );
	std::vector<std::string> thing = object.getTokens();
    SECTION( "index" ){
		REQUIRE( thing[0] == "server{" );
        REQUIRE( thing[1] == "listen" );
	}
}
