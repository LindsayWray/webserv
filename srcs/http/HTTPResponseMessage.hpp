#pragma once

#include <string>
#include <map>
#include <chrono>

class HTTPResponseMessage {
public:
	enum e_responseStatusCode {
		OK = 200,					/* SUCCESS */
		CREATED = 201,
		NO_CONTENT = 204,

		MOVED_PERMANENTLY = 301,	/* REDIRECTION */
		FOUND = 302,
		SEE_OTHER = 303,
		TEMPORARY_REDIRECT = 307,

		BAD_REQUEST = 400,			/* CLIENT ERRORS */
		FORBIDDEN = 403,
		NOT_FOUND = 404,
		REQUEST_TIMEOUT = 408,
		GONE = 410,
		IM_A_TEAPOT = 418,
		TOO_MANY_REQUESTS = 429,

		NOT_IMPLEMENTED = 501,		/* SERVER ERRORS */
		HTTP_VERSION_NOT_SUPPORTED = 505
		// ...
	};
	
	static const std::map<int, const std::string> responseStatusMessages;
	static const std::map<std::string, const std::string> contentTypes;

private:
	/** Status line */
	inline static const std::string	protocol = "HTTP/1.1";
	e_responseStatusCode			status;		// Need From RequestHandler
	std::string						message;

	/** Headers*/
	unsigned int 					length;		// Need From RequestHandler
	std::string						type;		// Need From RequestHandler
	std::string						_getDateStr() const;
	inline static const std::string server = "Wonderkind & Co's Webserver";

	/** Body */
	std::string						body;		// Need From RequestHandler

public:
	HTTPResponseMessage() {};
	~HTTPResponseMessage() {};

	/** How to use these adders:
	 *  HTTPResponseMessage response;
	 *						response.addStatus(SUCCESS)
	 *	 							.addTypeCat(TEXT)
	 * 								.addTypeExt("plain")
	 * 								.addLength(body.length())
	 *	 							.addBody(body);
	 **/
	HTTPResponseMessage&
	addStatus( const e_responseStatusCode status )
		{ this->status = status; this->message = responseStatusMessages.at(static_cast<int>(status)); return *this; }
	HTTPResponseMessage&
	addType( const std::string type ) { this->type = type; return *this; }
	HTTPResponseMessage&
	addLength( const unsigned int contentLength ) { length = contentLength; return *this; }
	HTTPResponseMessage&
	addBody( const std::string body ) { this->body = body; return *this; }

	const std::string toString() const;

private:
	const std::string _getStatusCodeStr() const;
};