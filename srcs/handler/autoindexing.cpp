#include "handler.hpp"
#include <dirent.h>

static void sorting( std::vector<std::string>& vec, std::string& body ) {
    std::sort( vec.begin(), vec.end() );
    for ( std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++ ) {
        body += * it;
    }
}

void webserv::autoIndexing( std::string path, std::string directory, std::string& body ) {
    std::vector<std::string> dirs;
    std::vector<std::string> files;
    DIR* dir;
    struct dirent* entry;

    std::cout << "going autoindexing " << directory << std::endl;
    if ( ( dir = opendir( directory.c_str() ) ) == NULL ) {
        throw ( webserv::DirectoryNotFoundException() );
    }
    readdir( dir );    // skip the first folder
    while ( ( entry = readdir( dir ) ) != NULL ) {
        if ( entry->d_type == DT_DIR ) {
            std::string line = "<a href=\"" + path + entry->d_name + "/\">"
                               + entry->d_name + "/"
                               + "</a><br>";
            dirs.push_back( line );
        } else {
            std::string line = "<a href=\"" + path + entry->d_name + "\">"
                               + entry->d_name
                               + "</a><br>";
            files.push_back( line );
        }
    }
    body += "<h2> Index of " + path + "</h2>";
    closedir( dir );
    sorting( dirs, body );
    sorting( files, body );
    return;
}
