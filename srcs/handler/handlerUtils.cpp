#include "handler.hpp"

using namespace webserv;

HTTPResponseMessage webserv::errorResponse( httpData server, HTTPResponseMessage::e_responseStatusCode code ) {
    HTTPResponseMessage response;
    std::string body = server.errorPage[code];
    return response.addStatus( code )
            .addLength( body.length() )
            .addBody( body )
            .addType( "text/html" );
}

HTTPResponseMessage webserv::responseWhenFileAlreadyExists( httpData server ) {
    HTTPResponseMessage response;
    std::string body = server.errorPage[403];
    return response.addStatus( HTTPResponseMessage::FORBIDDEN )
            .addType( "text/html" )
            .addLength( body.length() )
            .addBody( body );
}

HTTPResponseMessage webserv::responseWhenFileCreationFails( httpData server ) {
    HTTPResponseMessage response;
    std::string body = server.errorPage[403];
    return response.addStatus( HTTPResponseMessage::FORBIDDEN )
            .addType( "text/html" )
            .addLength( body.length() )
            .addBody( body );

}

HTTPResponseMessage webserv::responseWhenFileCreated( std::string requestURL ) {
    HTTPResponseMessage response;
    std::string body = "<html>\
						<body><h1>File created at URL: #</h1></body>\
						</head>\
						</html>";
    body.replace( body.find( "#" ), 1, requestURL );
    return response.addStatus( HTTPResponseMessage::CREATED )
            .addType( "text/html" )
            .addLength( body.length() )
            .addBody( body );

}

HTTPResponseMessage webserv::responseWhenFileDeleted( std::string requestURL ) {
    HTTPResponseMessage response;
    std::string body = "<html>\
						<body><h1>File deleted at URL: #</h1></body>\
						</head>\
						</html>";
    body.replace( body.find( "#" ), 1, requestURL );
    return response.addStatus( HTTPResponseMessage::OK )
            .addType( "text/html" )
            .addLength( body.length() )
            .addBody( body );
}

HTTPResponseMessage webserv::responseWhenFileCantBeDeleted( httpData server ) {
    HTTPResponseMessage response;
    std::string body = server.errorPage[403];
    return response.addStatus( HTTPResponseMessage::FORBIDDEN )
            .addType( "text/html" )
            .addLength( body.length() )
            .addBody( body );
}
