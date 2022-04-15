#include "handler.hpp"
#include <iostream>
#include <string>
#include <sys/stat.h>

using namespace webserv;

static std::string file_extension( std::string path ) {
    int pos = path.find( '.' );
    if ( pos == std::string::npos )
        return "";
    return path.substr( pos + 1 ); // +1 to skip the .
}

static HTTPResponseMessage responseFromFile( std::ifstream& file, std::string extension, HTTPResponseMessage::e_responseStatusCode statusCode ) {
    HTTPResponseMessage response;
    std::string line;
    std::string body( "" );

    while ( std::getline( file, line ) )
        body += ( line + '\n' );
    file.close();
    response.addStatus( statusCode )
            .addLength( body.length() )
            .addBody( body );
    try {
        response.addType( HTTPResponseMessage::contentTypes.at( extension ) );
    }
    catch ( ... ) {
        response.addType( "text/plain" );
    }
    return response;
}

static HTTPResponseMessage GET_handler( std::string path, httpData server, locationData location ) {
    std::ifstream file;
    HTTPResponseMessage response;
    std::string extension = file_extension( path );
    std::cout << "EXTENSION: " << extension << std::endl;
    std::string fullPath = location.root + path;

    std::cout << location.root << " " << path << " " << fullPath << "\n";

    if ( path.back() == '/' ) {
        std::cout << "Is a directory " << path << std::endl;
        if ( location.autoindex ) {
            std::string body;
            try {
                std::string concatPath;
                for ( int i = 0; i < location.path.size() - 1; i++ ) {
                    concatPath += location.path[i];
                }
                concatPath += path;
                autoIndexing( concatPath, fullPath, body );
            }
            catch ( DirectoryNotFoundException& e ) {
                std::cout << e.what() << std::endl;
                return errorResponse( server, HTTPResponseMessage::NOT_FOUND );
            }
            return response.addStatus( HTTPResponseMessage::OK )
                    .addBody( body )
                    .addLength( body.length() )
                    .addType( "text/html" );
        }
        return GET_handler( path + "index.html", server, location );
    }

    struct stat buf;
    if ( ::stat( fullPath.c_str(), & buf ) == -1 || !S_ISREG( buf.st_mode ) ) {
        return errorResponse( server, HTTPResponseMessage::NOT_FOUND );
    }
    file.open( fullPath );
    if ( file.good() ) {
        std::cout << "File found " << fullPath << std::endl;
        return responseFromFile( file, extension, HTTPResponseMessage::OK );
    } else {
        return errorResponse( server, HTTPResponseMessage::FORBIDDEN );
    }
}

static HTTPResponseMessage POST_handler( std::string requestPath, Request request, httpData server, locationData location ) {
    std::string extension = file_extension( requestPath );
    std::cout << "EXTENSION: " << extension << std::endl;
    std::string fullPath = location.root + requestPath;

    struct stat buf;
    bool fileAlreadyExists = ( ::stat( fullPath.c_str(), & buf ) != -1 );
    if ( fileAlreadyExists ) {
        return responseWhenFileAlreadyExists( server );
    } else {
        std::ofstream file;
        file.open( fullPath, std::ios::out | std::ios::binary );
        if ( file.good() ) {
            file << request.getBody();
            server.created_files.insert( fullPath );
        }
        file.close();
        return responseWhenFileCreated( requestPath );
    }
}

static HTTPResponseMessage DELETE_handler( std::string requestPath, httpData server, locationData location ) {
    std::string fullPath = location.root + requestPath;

    std::set<std::string>::iterator fileToBeDeleted = server.created_files.find( fullPath );
    if ( fileToBeDeleted != server.created_files.end() ) {
        if ( std::remove( fileToBeDeleted->c_str() ) == 0 ) {
            server.created_files.erase( fileToBeDeleted );
            return webserv::responseWhenFileDeleted( requestPath );
        } else {
            return responseWhenFileCantBeDeleted( requestPath );
        }
    } else {
        return responseWhenFileCantBeDeleted( requestPath );
    }
}

static HTTPResponseMessage REDIRECT_handler( Request request, httpData server ) {
    HTTPResponseMessage response;
    std::string requestPath;
    std::string location = server.redirect.second;
    int pos = location.find_first_of( "$uri" );
    if ( pos != std::string::npos ) {
        location.erase( pos, 4 );
        for ( int i = 0; i < request.getPath().size(); i++ )
            location.append( request.getPath()[i] );
    }
    response.addStatus( static_cast<HTTPResponseMessage::e_responseStatusCode>(server.redirect.first) )
            .addLength( 0 )
            .addBody( "" )
            .addLocation( location );
    return response;
}

HTTPResponseMessage webserv::handler( Request request, httpData server, locationData location ) {
    HTTPResponseMessage response;
    std::string requestPath;
    for ( int i = location.path.size() - 1; i < request.getPath().size(); i++ )
        requestPath += request.getPath()[i];

    if ( !location.allowed_response[request.getMethod()] )
        return errorResponse( server, HTTPResponseMessage::METHOD_NOT_ALLOWED );
    if ( server.redirect.first > 0 )
        return REDIRECT_handler( request, server );
    if ( request.getMethod() == Request::GET )
        return GET_handler( requestPath, server, location );
    else if ( request.getMethod() == Request::POST )
        return POST_handler( requestPath, request, server, location );
    else if ( request.getMethod() == Request::DELETE )
        return DELETE_handler( requestPath, server, location );
    return response;
}
