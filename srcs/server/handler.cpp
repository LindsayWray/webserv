#include "server.hpp"
#include <iostream>
#include <string>
#include <sys/stat.h>

using webserv::Request;

static std::string file_extension( std::string path ) {
    int pos = path.find( '.' );
    if ( pos == std::string::npos )
        return "";
    return path.substr( pos + 1 ); // +1 to skip the .
}

static HTTPResponseMessage
responseFromFile( std::ifstream &file, std::string extension, HTTPResponseMessage::e_responseStatusCode statusCode ) {
    HTTPResponseMessage response;
    std::string line;
    std::string body( "" );

    while ( std::getline( file, line ))
        body += ( line + '\n' );
    file.close();
    response.addStatus( statusCode )
            .addLength( body.length())
            .addBody( body );
    try {
        response.addType( HTTPResponseMessage::contentTypes.at( extension ));
    }
    catch ( ... ) {
        response.addType( "text/plain" );   //temporary fix until directory handling
    }
    return response;
}

HTTPResponseMessage errorResponse( webserv::httpData *config, HTTPResponseMessage::e_responseStatusCode code ) {
    HTTPResponseMessage response;
    std::string body = config->error_page[code];
    return response.addStatus( code )
            .addLength( body.length())
            .addBody( body )
            .addType( "text/html" );
}

HTTPResponseMessage GET_handler( std::string path, webserv::httpData *config, webserv::locationData *location ) {
    std::ifstream file;
    HTTPResponseMessage response;
    std::string extension = file_extension( path );
    std::cout << "EXTENSION: " << extension << std::endl;
    std::string fullPath = location->root + path;

    std::cout << location->root << " " << path << " " << fullPath << "\n";

    if ( path.back() == '/' ) {
        std::cout << "Is a directory " << path << std::endl;
        if ( location->autoindex ) {
            std::string body;
            try {
                std::string concatPath;
                for ( int i = 0; i < location->path.size() - 1; i++ ) {
                    concatPath += location->path[i];
                }
                concatPath += path;
                autoIndexing( concatPath, fullPath, body );
            }
            catch ( DirectoryNotFoundException &e ) {
                std::cout << e.what() << std::endl;
                return errorResponse( config, HTTPResponseMessage::NOT_FOUND );
            }
            return response.addStatus( HTTPResponseMessage::OK )
                    .addBody( body )
                    .addLength( body.length())
                    .addType( "text/html" );
        }
        return GET_handler( path + "index.html", config, location );
    }

    struct stat buf;
    if ( ::stat( fullPath.c_str(), &buf ) == -1 || !S_ISREG( buf.st_mode )) {
        return errorResponse( config, HTTPResponseMessage::NOT_FOUND );
    }
    file.open( fullPath );
    if ( file.good()) {
        std::cout << "File found " << fullPath << std::endl;
        return responseFromFile( file, extension, HTTPResponseMessage::OK );
    } else {
        return errorResponse( config, HTTPResponseMessage::FORBIDDEN );
    }
}

HTTPResponseMessage responseWhenFileAlreadyExists( webserv::httpData *config ) {
    HTTPResponseMessage response;
    std::string body = config->error_page[405];
    return response.addStatus( HTTPResponseMessage::METHOD_NOT_ALLOWED )
            .addType( "text/html" )
            .addLength( body.length())
            .addBody( body );
}

HTTPResponseMessage responseWhenFileCreated( std::string &requestURL ) {
    HTTPResponseMessage response;
    std::string body = "<html>\
						<body><h1>File created at URL: #</h1></body>\
						</head>\
						</html>";
    body.replace( body.find( "#" ), 1, requestURL );
    return response.addStatus( HTTPResponseMessage::CREATED )
            .addType( "text/html" )
            .addLength( body.length())
            .addBody( body );

}

HTTPResponseMessage
POST_handler( std::string &requestPath, Request request, webserv::httpData *config, webserv::locationData *location ) {
    std::string extension = file_extension( requestPath );
    std::cout << "EXTENSION: " << extension << std::endl;
    std::string fullPath = location->root + requestPath;

    struct stat buf;
    bool fileAlreadyExists = ( ::stat( fullPath.c_str(), &buf ) != -1 );
    if ( fileAlreadyExists ) {
        return responseWhenFileAlreadyExists( config );
    } else {
        std::ofstream file;
        file.open( fullPath, std::ios::out | std::ios::binary );
        if ( file.good()) {
            file << request.getBody();
            config->created_files.insert( fullPath );
        }
        file.close();
        return responseWhenFileCreated( requestPath );
    }
}

HTTPResponseMessage responseWhenFileDeleted( std::string &requestURL ) {
    HTTPResponseMessage response;
    std::string body = "<html>\
						<body><h1>File deleted at URL: #</h1></body>\
						</head>\
						</html>";
    body.replace( body.find( "#" ), 1, requestURL );
    return response.addStatus( HTTPResponseMessage::OK )
            .addType( "text/html" )
            .addLength( body.length())
            .addBody( body );
}

HTTPResponseMessage responseWhenFileCantBeDeleted( std::string &requestURL ) {
    HTTPResponseMessage response;
    std::string body = "<html>\
                        <body><h1>File cannot be deleted at URL: #</h1></body>\
                        </head>\
                        </html>";
    body.replace( body.find( "#" ), 1, requestURL );
    return response.addStatus( HTTPResponseMessage::OK )
            .addType( "text/html" )
            .addLength( body.length())
            .addBody( body );
}

HTTPResponseMessage DELETE_handler( std::string &requestPath, Request request, webserv::httpData *config,
                                    webserv::locationData *location ) {
    std::string fullPath = location->root + requestPath;

    std::set<std::string>::iterator fileToBeDeleted = config->created_files.find( fullPath );
    if ( fileToBeDeleted != config->created_files.end()) {
        if ( std::remove( fileToBeDeleted->c_str()) == 0 ) {
            config->created_files.erase( fileToBeDeleted );
            return responseWhenFileDeleted( requestPath );
        } else {
            return responseWhenFileCantBeDeleted( requestPath );
        }
    } else {
        return responseWhenFileCantBeDeleted( requestPath );
    }
}


HTTPResponseMessage REDIRECT_handler( Request request, webserv::httpData *config ) {
    HTTPResponseMessage response;
    std::string requestPath;
    std::string location = config->redirect.second;
    int pos = location.find_first_of( "$uri" );
    if ( pos != std::string::npos ) {
        location.erase( pos, 4 );
        for ( int i = 0; i < request.getPath().size(); i++ )
            location.append( request.getPath()[i] );
    }
    response.addStatus( static_cast<HTTPResponseMessage::e_responseStatusCode>(config->redirect.first))
            .addLength( 0 )
            .addBody( "" )
            .addLocation( location );
    return response;
}

HTTPResponseMessage handler( Request request, webserv::httpData *config, webserv::locationData location ) {
    HTTPResponseMessage response;
    std::string requestPath;
    for ( int i = location.path.size() - 1; i < request.getPath().size(); i++ ) {
        requestPath += request.getPath()[i];
    }

    if ( config->redirect.first > 0 )
        return REDIRECT_handler( request, config );
    if ( request.getMethod() == Request::GET )
        return GET_handler( requestPath, config, &location );
    else if ( request.getMethod() == Request::POST )
        return POST_handler( requestPath, request, config, &location );
    else if ( request.getMethod() == Request::DELETE )
        return DELETE_handler( requestPath, request, config, &location );
    return response;
}