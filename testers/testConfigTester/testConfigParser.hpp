//
// Created by Kas on 20/03/2022.
//

#ifndef WEBSERV_TESTCONFIGPARSER_HPP
#define WEBSERV_TESTCONFIGPARSER_HPP

#include "../../srcs/server/server.hpp"

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

namespace webserv{

	template< class T >
	void vec_test( std::vector< T > expected, std::vector< T > result, std::string type ){
		bool same = true;
		typename std::vector< T >::iterator it_bar = expected.begin();
		typename std::vector< T >::iterator it_in = result.begin();

		std::cout << CYAN << "\n---- " << type << " test ----" << RESET << std::endl;
		if ( expected.size() != result.size()){
			std::cout << " elements expected: " << expected.size() << " result: " << result.size() << std::endl;
			same = false;
		}
		for ( ; it_bar != expected.end() && it_in != result.end(); it_bar++ ){
			if ( *it_bar != *it_in ){
				same = false;
				std::cout << " " << type << " not same expected: " << *it_bar << " result: " << *it_in << std::endl;
			}
			it_in++;
		}
		for ( ; it_in != result.end(); it_in++ )
			std::cout << " to many elements in vector: " << *it_in << std::endl;
		if ( !same )
			std::cout << BOLDRED << "**  not same result **" << RESET << std::endl;
		else
			std::cout << BOLDGREEN << "** Test passed!! **" << RESET << std::endl;
	}

    template< class T >
    void vec_test_pair( std::vector< T > expected, std::vector< T > result, std::string type ){
        bool same = true;
        typename std::vector< T >::iterator it_bar = expected.begin();
        typename std::vector< T >::iterator it_in = result.begin();

        std::cout << CYAN << "\n---- " << type << " test ----" << RESET << std::endl;
        if ( expected.size() != result.size()){
            std::cout << " elements expected: " << expected.size() << " result: " << result.size() << std::endl;
            same = false;
        }
        for ( ; it_bar != expected.end() && it_in != result.end(); it_bar++ ){
            if ( it_bar->first != it_in->first ){
                same = false;
                std::cout << " first element " << type << " not same expected: " << it_bar->first << " result: " << it_in->first << std::endl;
            }
            if ( it_bar->second != it_in->second ){
                same = false;
                std::cout << " second element " << type << " not same expected: " << it_bar->second << " result: " << it_in->second << std::endl;
            }
            it_in++;
        }
        for ( ; it_in != result.end(); it_in++ )
            std::cout << " to many elements in vector: " << it_in->first << " " << it_in->second << std::endl;
        if ( !same )
            std::cout << BOLDRED << "**  not same result **" << RESET << std::endl;
        else
            std::cout << BOLDGREEN << "** Test passed!! **" << RESET << std::endl;
    }
	template< class T, class U >
	void vec_test_map( std::map< T, U > expected, std::map< T, U > result, std::string type ){
		bool same = true;
		typename std::map< T, U >::iterator it_bar = expected.begin();
		typename std::map< T, U >::iterator it_in = result.begin();

		std::cout << CYAN << "\n---- " << type << " test ----" << RESET << std::endl;
		if ( expected.size() != result.size()){
			std::cout << " elements expected: " << expected.size() << " result: " << result.size() << std::endl;
			same = false;
		}
		for ( ; it_bar != expected.end() && it_in != result.end(); it_bar++ ){
			if ( it_bar->first != it_in->first ){
				same = false;
				std::cout << " first element " << type << " not same expected: " << it_bar->first << " result: " << it_in->first << std::endl;
			}
			if ( it_bar->second != it_in->second ){
				same = false;
				std::cout << " second element " << type << " not same expected: " << it_bar->second << " result: " << it_in->second << std::endl;
			}
			it_in++;
		}
		for ( ; it_in != result.end(); it_in++ )
			std::cout << " to many elements in vector: " << it_in->first << " " << it_in->second << std::endl;
		if ( !same )
			std::cout << BOLDRED << "**  not same result **" << RESET << std::endl;
		else
			std::cout << BOLDGREEN << "** Test passed!! **" << RESET << std::endl;
	}

	void StaticSocketData( socketData _socket, int dom, int serv, int proto, u_long inter, int bl, int wc ){
		bool same = true;
		std::cout << CYAN << "\n---- Static socket data test ----" << RESET << std::endl;

		if ( dom != _socket.domain ){
			same = false;
			std::cout << " domain is not same, expected: " << dom << " result: " << _socket.domain << std::endl;
		}
		if ( serv != _socket.service ){
			same = false;
			std::cout << " service is not same, expected: " << serv << " result: " << _socket.service << std::endl;
		}
		if ( proto != _socket.protocol ){
			same = false;
			std::cout << " protocol is not same, expected: " << proto << " result: " << _socket.protocol << std::endl;
		}
		if ( inter != _socket.interface ){
			same = false;
			std::cout << " interface is not same, expected: " << inter << " result: " << _socket.interface << std::endl;
		}
		if ( bl != _socket.backlog ){
			same = false;
			std::cout << " backlog is not same, expected: " << bl << " result: " << _socket.backlog << std::endl;
		}
		if ( wc != _socket.worker_connections ){
			same = false;
			std::cout << " worker_connections is not same, expected: " << wc << " result: "
					  << _socket.worker_connections << std::endl;
		}
		if ( !same )
			std::cout << BOLDRED << "**  not same result **" << RESET << std::endl;
		else
			std::cout << BOLDGREEN << "** Test passed!! **" << RESET << std::endl;
	}

	void testLocation( locationData loc, std::vector<std::string> location, std::string root, /*int allowed_respose[3],*/ bool autoindex, std::string cgi_param, bool CGI ){
		bool same = true;
		std::cout << CYAN << "\n---- Location data test ----" << RESET << std::endl;
		for (int i = 0; i < loc.path.size(); i++ )
            if ( location[i] != loc.path[i] ){
                same = false;
                std::cout << " location is not same, expected: " << location[i] << " result: " << loc.path[i] << std::endl;
            }
		if ( root != loc.root ){
			same = false;
			std::cout << " root is not same, expected: " << root << " result: " << loc.root << std::endl;
		}
        if ( cgi_param != loc.cgi_param ){
            same = false;
            std::cout << " cgi_param is not same, expected: " << cgi_param << " result: " << loc.cgi_param << std::endl;
        }
//		for ( int i = 0; i < 3; i++ )
//			if ( allowed_respose[i] != loc.allowed_response[i] ){
//				same = false;
//				std::cout << " allowed_response is not same, expected: " << allowed_respose[i] << " result: " << loc.allowed_respose[i] << std::endl;
//			}
		if ( autoindex != loc.autoindex ){
			same = false;
			std::cout << " autoindex is not same, expected: " << autoindex << " result: " << loc.autoindex << std::endl;
		}
        if ( CGI != loc.CGI ){
            same = false;
            std::cout << " CGI is not same, expected: " << CGI << " result: " << loc.CGI << std::endl;
        }
		if ( !same )
			std::cout << BOLDRED << "**  not same result **" << RESET << std::endl;
		else
			std::cout << BOLDGREEN << "** Test passed!! **" << RESET << std::endl;
	}
}
#endif //WEBSERV_TESTCONFIGPARSER_HPP
