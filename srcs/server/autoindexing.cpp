#include "server.hpp"
#include <dirent.h>


void	sorting(std::vector<std::string>& vec, std::string& body){
	std::sort(vec.begin(), vec.end());
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++) {
		body += *it;
	}
}

void	autoIndexing( std::string path, std::string directory, std::string& body ) {
	std::vector<std::string> dirs;
	std::vector<std::string> files;
	DIR *dir;
	struct dirent *entry;

	std::cout << "going autoindexing " << directory << std::endl;
	if ((dir = opendir(directory.c_str())) == NULL) {
		throw (DirectoryNotFoundException());
	}
	readdir(dir);	// skip the first folder
	std::cout << "passing by \n" << std::endl;
	while ((entry = readdir(dir)) != NULL) {
		std::string line = "<a href=\"" + path + entry->d_name + "\">";
		line += entry->d_name;
		if ( entry->d_type == DT_DIR ){
			line += "/\n";
			line += "</a><br>";
			dirs.push_back(line);
		} else {
			line += "</a><br>";
			files.push_back(line);
		}
	}
	closedir (dir);
	sorting(dirs, body);
	sorting(files, body);
	return ;
}
