//
// Created by Kester kas De rooij on 4/6/22.
//

#include "dataStructs.hpp"

/*
 * socketData struct method functions
 */

webserv::socketData::socketData( void ) {
    domain = AF_INET;
    service = SOCK_STREAM;
    protocol = 0;
    //ports.push_back( 80 );
    interface = INADDR_ANY;
    backlog = 32;
    worker_connections = 1024;
}

void webserv::socketData::addPort( int newPort ) { // start on port 80, if no port 80 is defined use other
    if ( std::find( ports.begin(), ports.end(), newPort ) == ports.end())
        ports.push_back( newPort );
}

/*
 * locationData struct method functions
 */

webserv::locationData::locationData( std::string _root ) {
    path.push_back( "/" );
    root = _root;
    cgi_param = "NONE";
    memset( allowed_response, true, 3 );
    autoindex = false;
    CGI = false;
}

int webserv::locationData::tokenizer( std::string line ) {
    std::size_t i = 0, found;

    if ( line[i++] != '/' )
        return ERROR;
    while ( i < line.length()) {
        found = line.find_first_of( "/", i );
        if ( found == std::string::npos ) {
            path.push_back( line.substr( i, line.length()));
            break;
        } else {
            if ( i != found - i )
                path.push_back( line.substr( i, found - i ));
            path.push_back( line.substr( found, 1 ));
            i = found + 1;
        }
    }
    return SUCCES;
}

/*
 * httpData struct method functions
 */

webserv::httpData::httpData( void ) { return; }

webserv::httpData::httpData( std::string root ) : abs_path( root ), redirect( std::make_pair( -1, "" )) {
    max_client_body_size = 0;
}

webserv::httpData::httpData(const httpData& original){
    *this = original;
}

webserv::httpData& webserv::httpData::operator=(const webserv::httpData& original){
    this->env = original.env;
    this->abs_path = original.abs_path;
    this->server_name = original.server_name;
    this->index = original.index;
    this->error_page = original.error_page;
    this->redirect = original.redirect;
    this->locations = original.locations;
    this->max_client_body_size = original.max_client_body_size;
    this->port = original.port;
    this->created_files = original.created_files;
    return *this;
}

webserv::httpData::~httpData() {}

std::map<std::string, std::string> created_files;

std::string webserv::httpData::formatErrorPage( std::string message ) {
    std::string page = "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\
                                <html>\
                                <head>\
                                <title>#</title>\
                                <body><h1>Default error page: $</h1></body>\
                                </head>\
                                </html>";
    page.replace( page.find( "#" ), 1, message );
    page.replace( page.find( "$" ), 1, message );
    return page;
}

void webserv::httpData::setErrorPages( void ) {
    if ( error_page.find( 400 ) == error_page.end())
        error_page[400] = formatErrorPage( "400 Bad Request" );
    if ( error_page.find( 403 ) == error_page.end())
        error_page[403] = formatErrorPage( "403 Forbidden" );
    if ( error_page.find( 404 ) == error_page.end())
        error_page[404] = formatErrorPage( "404 Not Found" );
    if ( error_page.find( 405 ) == error_page.end())
        error_page[405] = formatErrorPage( "405 Method Not Allowed" );
    if ( error_page.find( 408 ) == error_page.end())
        error_page[408] = formatErrorPage( "408 Request Timeout" );
    if ( error_page.find( 411 ) == error_page.end())
        error_page[411] = formatErrorPage( "411 Length Required" );
	if ( error_page.find( 413 ) == error_page.end())
        error_page[413] = formatErrorPage( "413 Payload Too Large" );
    if ( error_page.find( 414 ) == error_page.end())
        error_page[414] = formatErrorPage( "414 URI Too Long" );
    if ( error_page.find( 418 ) == error_page.end())
        error_page[418] = formatErrorPage( "418 I'm a Teapot" );
    if ( error_page.find( 429 ) == error_page.end())
        error_page[429] = formatErrorPage( "429 Too Many Requests" );
    if ( error_page.find( 431 ) == error_page.end())
        error_page[431] = formatErrorPage( "431 Request Header Fields Too Large" );
    if ( error_page.find( 500 ) == error_page.end())
        error_page[500] = formatErrorPage( "500 Internal Server Error" );
    if ( error_page.find( 501 ) == error_page.end())
        error_page[501] = formatErrorPage( "501 Not Implemented" );
    if ( error_page.find( 505 ) == error_page.end())
        error_page[505] = formatErrorPage( "505 HTTP Version Not Supported" );
}
