#include "catch.hpp"
#include "../config/parentConfig.hpp"

#define MAX_VAL 2345

TEST_CASE( "configParsing", "[parentConfig]" ){
	int a = 1;
	int b = 1;
    SECTION( "index" ){
		REQUIRE( a == 1 );
        REQUIRE( b == 1 );
	}
}
