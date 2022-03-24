//
// Created by Kas on 20/03/2022.
//

#ifndef WEBSERV_STRINGUTILS_HPP
#define WEBSERV_STRINGUTILS_HPP


namespace webserv{

	int findPWD( char** env ){
		std::string pwd = "PWD=";
		int i = 0, found;

		for (; env[i]; i++ ){
			found = 1;
			for ( int j = 0; j < 4 && env[i][j]; j++){
				if ( env[i][j] != pwd[j])
					found = 0;
			}
			if ( found )
				break;
		}
		if ( !found )
			return -1;
		return i;
	}

	std::string setFileLocation( char** env ){
		int pwd = findPWD( env );
		if ( pwd == ERROR )
			return "PWDNOTFOUND";
		std::string current( env[pwd] );
		int pos = current.find("webserv");
		std::string root = current.substr(4, pos + 3  );
		return root;
	}

}
#endif //WEBSERV_STRINGUTILS_HPP
