//
// Created by Kas on 17/03/2022.
//

#include "configParser.hpp"
#include "../../srcs/utils/stringUtils.hpp"
#include "testConfigParser.hpp"

template<typename T>
std::vector<T>& operator << (std::vector<T>& v, const T & item){
	v.push_back(item);  return v;
}
template<typename T>
std::vector<T>& operator,(std::vector<T>& v, const T & item){
	v.push_back(item);  return v;
}

void testOneServer( int ret, webserv::httpData http, webserv::socketData socket, char** env ){
	std::vector<int> ports;
	std::vector<std::string> names;
	std::vector<std::pair<int,std::string>> pairs;

	ports << 80, 3000, 20;
	webserv::vec_test( ports, socket.ports, "Ports" );
	webserv::StaticSocketData( socket, AF_INET, SOCK_STREAM, 0, INADDR_ANY, 32, 10 );
	names << (std::string) "webserv.com",  (std::string)"www.webserv.com";
	webserv::vec_test( names, http.server_name, "server_names" );
	names.clear();
	names << (std::string) "index.html",  (std::string)"index.htm";
	names << (std::string) "index.hh",  (std::string)"index";
	webserv::vec_test( names, http.index, "Index" );
	pairs << std::make_pair(404, (std::string)"/error.html");
	pairs << std::make_pair(303, (std::string)"/error.hh");
	webserv::vec_test_pair( pairs, http.error_page, "error_page" );
	pairs.clear();
	pairs << std::make_pair(302, (std::string)"http://webserv.com$url");
	pairs << std::make_pair(303, (std::string)"http://webserv.com$url");
	webserv::vec_test_pair( pairs, http.redirect, "redirect" );
	if ( ret < 0 )
		return;
	webserv::testLocation(http.locations[0], "/", "/var/www/site1", true );
	webserv::testLocation(http.locations[1], "/www/", "/var/www/site4", false );
	webserv::testLocation(http.locations[2], "/", "/var/www/site1", true );
}

void clearAll( webserv::httpData* http, webserv::socketData* socket ){
	socket->ports.clear();
	http->locations.clear();
	http->index.clear();
	http->error_page.clear();
	http->redirect.clear();
	http->server_name.clear();
}

int main( int argc, char** argv, char** env ){
    std::string root = webserv::setFileLocation( env );
	webserv::httpData http1( root );
    webserv::httpData http2( root );
    webserv::httpData http3( root );
    webserv::httpData http4( root );
    webserv::httpData http5( root );
    root.append("/testers/testConfigTester/");
	std::string configFile1 = root;
	std::string configFile2 = root;
	configFile1.append( "testfile1.webserv" );
    configFile2.append( "testfile2.webserv" );
	webserv::configParser test1(configFile1 );
	webserv::configParser test2(configFile2 );
	webserv::socketData socket1, socket2, socket3, socket4, socket5;
	int ret;

	std::cout << BOLDYELLOW << "testfile 1, 1 server" << RESET << std::endl;
	ret = test1.parseIntoPieces( &socket1, &http1 );
	testOneServer( ret, http1, socket1, env );
	std::cout << BLUE << "ret parsing: " << ret << " expected: " << SUCCES << RESET << std::endl;

	std::cout << BOLDYELLOW << "\ntestfile 2, 3 server" << RESET << std::endl;
	ret = test2.parseIntoPieces( &socket2, &http2 );
	testOneServer( ret, http2, socket2, env );
	std::cout << BLUE << "server: 1 ret parsing: " << ret << " expected: " << NEOF << RESET << std::endl;
	ret = test2.parseIntoPieces( &socket3, &http3 );
	testOneServer( ret, http3, socket3, env );
	std::cout << BLUE << "server: 2 ret parsing: " << ret << " expected: " << NEOF << RESET << std::endl;
	ret = test2.parseIntoPieces( &socket4, &http4 );
	testOneServer( ret, http4, socket4, env );
	std::cout << BLUE << "server: 3 ret parsing: " << ret << " expected: " << SUCCES << RESET << std::endl;
	ret = test2.parseIntoPieces( &socket5, &http5 );
	std::cout << BLUE << "server: NAV ret parsing: " << ret << " expected: " << ERROR << RESET << std::endl;

	std::cout << CYAN << "\n---- check every variable without ; ----" << RESET << std::endl;

    std::string configFile3 = root;
    configFile3.append( "testfile3_1.webserv" );
	webserv::configParser test3_1 (configFile3 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 1 parsing: " << test3_1.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile3 = root;
    configFile3.append( "testfile3_2.webserv" );
	webserv::configParser test3_2 (configFile3 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 2 parsing: " << test3_2.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile3 = root;
    configFile3.append( "testfile3_3.webserv" );
	webserv::configParser test3_3 (configFile3 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 3 parsing: " << test3_3.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile3 = root;
    configFile3.append( "testfile3_4.webserv" );
	webserv::configParser test3_4 (configFile3 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 4 parsing: " << test3_4.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile3 = root;
    configFile3.append( "testfile3_5.webserv" );
	webserv::configParser test3_5 (configFile3 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 5 parsing: " << test3_5.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile3 = root;
    configFile3.append( "testfile3_6.webserv" );
	webserv::configParser test3_6 (configFile3 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 6 parsing: " << test3_6.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile3 = root;
    configFile3.append( "testfile3_7.webserv" );
	webserv::configParser test3_7 (configFile3 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 7 parsing: " << test3_7.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile3 = root;
    configFile3.append( "testfile3_8.webserv" );
	webserv::configParser test3_8 (configFile3 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 8 parsing: " << test3_8.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile3 = root;
    configFile3.append( "testfile3_9.webserv" );
	webserv::configParser test3_9 (configFile3 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: reference parsing: " << test3_9.parseIntoPieces( &socket5, &http5 ) << " expected: " << SUCCES << RESET << std::endl;

	std::cout << CYAN << "\n---- check every variable without ; ----" << RESET << std::endl;

	std::string configFile4 = root;
    configFile4.append( "testfile4_1.webserv" );
	webserv::configParser test4_1 (configFile4 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 1 parsing: " << test4_1.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile4 = root;
    configFile4.append( "testfile4_2.webserv" );
	webserv::configParser test4_2 (configFile4 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 2 parsing: " << test4_2.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile4 = root;
    configFile4.append( "testfile4_3.webserv" );
	webserv::configParser test4_3 (configFile4 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 3 parsing: " << test4_3.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile4 = root;
    configFile4.append( "testfile4_4.webserv" );
	webserv::configParser test4_4 (configFile4 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 4 parsing: " << test4_4.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile4 = root;
    configFile4.append( "testfile4_5.webserv" );
	webserv::configParser test4_5 (configFile4 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 5 parsing: " << test4_5.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile4 = root;
    configFile4.append( "testfile4_6.webserv" );
	webserv::configParser test4_6 (configFile4 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 6 parsing: " << test4_6.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;

    configFile4 = root;
    configFile4.append( "testfile4_7.webserv" );
	webserv::configParser test4_7 (configFile4 );
	clearAll( &http5, &socket5 );
	std::cout << BLUE << "server: 7 parsing: " << test4_7.parseIntoPieces( &socket5, &http5 ) << " expected: " << ERROR << RESET << std::endl;
}