#pragma once

#include <string>
#include <ctime>

class HTTPResponseMessage {
public:
	enum responseStatusCode { SUCCESS, REDIRECTION, BAD_REQUEST, FORBIDDEN, NOT_FOUND };
	static std::string responseStatusMessages[];

	// content-type constitutes text/html, image/png, image/jpeg, application/json and more.
	enum contentTypeCategory { APPLICATION, AUDIO, IMAGE, TEXT };

private:
	/** Status line */
	const std::string	protocol = "HTTP/1.1";
	responseStatusCode			status;		// Need From RequestHandler
	std::string					message;

	/** Headers*/
	const time_t 		date = std::time(NULL);
	contentTypeCategory			typeCat;	// Need From RequestHandler
	std::string					typeExt;	// Need From RequestHandler
	unsigned int 				length;		// Need From RequestHandler
	const std::string 	server = "Wonderkid & Co's Webserver";

	/** Body */
	std::string					body;		// Need From RequestHandler (string? stream? file*?)

public:
	HTTPResponseMessage() {};
	~HTTPResponseMessage() {};

	void setStatus( const responseStatusCode status )
		{ this->status = status; this->message = responseStatusMessages[status]; }
	void setTypeCat( const contentTypeCategory typeCat ) { this->typeCat = typeCat; }
	void setTypeExt( const std::string& typeExt ) { this->typeExt = typeExt; }
	void setLength( const unsigned int contentLength ) { length = contentLength; }
	void setBody( const std::string& body ) { this->body = body; }

	/** In case you want to try out adders instead of setters:
	 *  HTTPResponseMessage response()
	 *								.addStatus(SUCCESS)
	 *	 							.addStatus(SUCCESS)
	 * 								.addTypeExt("plain")
	 * 								.addLength(body.length())
	 *	 							.addBody(body);
	 **/
	HTTPResponseMessage&
	addStatus( const responseStatusCode status )
		{ this->status = status; this->message = responseStatusMessages[status]; return *this; }
	HTTPResponseMessage&
	addTypeCat( const contentTypeCategory typeCat ) { this->typeCat = typeCat; return *this; }
	HTTPResponseMessage&
	addTypeExt( const std::string& typeExt ) { this->typeExt = typeExt; return *this; }
	HTTPResponseMessage&
	addLength( const unsigned int contentLength ) { length = contentLength; return *this; }
	HTTPResponseMessage&
	addBody( const std::string& body ) { this->body = body; return *this; }
};