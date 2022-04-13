#include "dataStructs.hpp"

using namespace webserv;

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
    while ( i < line.length() ) {
        found = line.find_first_of( "/", i );
        if ( found == std::string::npos ) {
            path.push_back( line.substr( i, line.length() ) );
            break;
        } else {
            if ( i != found - i )
                path.push_back( line.substr( i, found - i ) );
            path.push_back( line.substr( found, 1 ) );
            i = found + 1;
        }
    }
    return SUCCES;
}

/*
 * httpData struct method functions
 */

webserv::httpData::httpData( void ) { return; }

webserv::httpData::httpData( std::string root ) : absPath( root ), redirect( std::make_pair( -1, "" ) ) {
    maxClientBody = 0;
}

webserv::httpData::httpData( const httpData& original ) {
    * this = original;
}

httpData& webserv::httpData::operator=( const httpData& original ) {
    this->env = original.env;
    this->absPath = original.absPath;
    this->serverName = original.serverName;
    this->index = original.index;
    this->errorPage = original.errorPage;
    this->redirect = original.redirect;
    this->locations = original.locations;
    this->maxClientBody = original.maxClientBody;
    this->port = original.port;
    this->created_files = original.created_files;
    return * this;
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
    if ( errorPage.find( 400 ) == errorPage.end() )
        errorPage[400] = formatErrorPage( "400 Bad Request" );
    if ( errorPage.find( 403 ) == errorPage.end() )
        errorPage[403] = formatErrorPage( "403 Forbidden" );
    if ( errorPage.find( 404 ) == errorPage.end() )
        errorPage[404] = formatErrorPage( "404 Not Found" );
    if ( errorPage.find( 405 ) == errorPage.end() )
        errorPage[405] = formatErrorPage( "405 Method Not Allowed" );
    if ( errorPage.find( 408 ) == errorPage.end() )
        errorPage[408] = formatErrorPage( "408 Request Timeout" );
    if ( errorPage.find( 411 ) == errorPage.end() )
        errorPage[411] = formatErrorPage( "411 Length Required" );
    if ( errorPage.find( 413 ) == errorPage.end() )
        errorPage[413] = formatErrorPage( "413 Payload Too Large" );
    if ( errorPage.find( 414 ) == errorPage.end() )
        errorPage[414] = formatErrorPage( "414 URI Too Long" );
    if ( errorPage.find( 418 ) == errorPage.end() )
        errorPage[418] = formatErrorPage( "418 I'm a Teapot" );
    if ( errorPage.find( 429 ) == errorPage.end() )
        errorPage[429] = formatErrorPage( "429 Too Many Requests" );
    if ( errorPage.find( 431 ) == errorPage.end() )
        errorPage[431] = formatErrorPage( "431 Request Header Fields Too Large" );
    if ( errorPage.find( 500 ) == errorPage.end() )
        errorPage[500] = formatErrorPage( "500 Internal Server Error" );
    if ( errorPage.find( 501 ) == errorPage.end() )
        errorPage[501] = formatErrorPage( "501 Not Implemented" );
    if ( errorPage.find( 505 ) == errorPage.end() )
        errorPage[505] = formatErrorPage( "505 HTTP Version Not Supported" );
}
