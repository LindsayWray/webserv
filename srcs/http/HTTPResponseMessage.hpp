#pragma once

#include <string>
#include <vector>
#include <ctime>

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
	// Not sure if responseStatusMessages compiles. Otherwise data initialization should be outside the class scope inside a cpp file
	static const std::vector<const std::string> responseStatusMessages;

	// content-type examples constitute text/html, image/png, image/jpeg, application/json and more.
	enum e_contentTypeCategory { APPLICATION, AUDIO, IMAGE, TEXT /* , ETC... */ };

private:
	/** Status line */
	inline static const std::string	protocol = "HTTP/1.1";
	e_responseStatusCode			status;		// Need From RequestHandler
	const std::string*				message;

	/** Headers*/
	inline static const time_t 		date = std::time(NULL);
	e_contentTypeCategory			typeCat;	// Need From RequestHandler
	const std::string*				typeExt;	// Need From RequestHandler
	unsigned int 					length;		// Need From RequestHandler
	inline static const std::string server = "Wonderkid & Co's Webserver";

	/** Body */
	const std::string*				body;		// Need From RequestHandler (string? stream? file*?)

public:
	HTTPResponseMessage() {};
	~HTTPResponseMessage() {};

	void setStatus( const e_responseStatusCode status )
		{ this->status = status; this->message = &responseStatusMessages[status]; }
	void setTypeCat( const e_contentTypeCategory typeCat ) { this->typeCat = typeCat; }
	void setTypeExt( const std::string* typeExt ) { this->typeExt = typeExt; }
	void setLength( const unsigned int contentLength ) { length = contentLength; }
	void setBody( const std::string* body ) { this->body = body; }

	/** In case you want to try out adders instead of setters:
	 *  HTTPResponseMessage response()
	 *								.addStatus(SUCCESS)
	 *	 							.addTypeCat(TEXT)
	 * 								.addTypeExt("plain")
	 * 								.addLength(body.length())
	 *	 							.addBody(body);
	 **/
	HTTPResponseMessage&
	addStatus( const e_responseStatusCode status )
		{ this->status = status; this->message = &responseStatusMessages[status]; return *this; }
	HTTPResponseMessage&
	addTypeCat( const e_contentTypeCategory typeCat ) { this->typeCat = typeCat; return *this; }
	HTTPResponseMessage&
	addTypeExt( const std::string* typeExt ) { this->typeExt = typeExt; return *this; }
	HTTPResponseMessage&
	addLength( const unsigned int contentLength ) { length = contentLength; return *this; }
	HTTPResponseMessage&
	addBody( const std::string* body ) { this->body = body; return *this; }

	std::string& format() const {
		std::string output;

		output = protocol + ' ' + std::to_string((int)status);

		return output;
	}


};
