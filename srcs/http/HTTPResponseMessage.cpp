#include "HTTPResponseMessage.hpp"

const std::map<int, const std::string> HTTPResponseMessage::responseStatusMessages =
{
	{ 200, "OK" }, 					/* SUCCESS */
	{ 201, "Created" },
	{ 204, "No Content" },

	{ 301, "Moved Permanently"},	/* REDIRECTION */
	{ 302, "Found"}, 
	{ 303, "See Other"},
	{ 307, "Temporary Redirect"},

	{ 400, "Bad Request"},			/* CLIENT ERRORS */
	{ 403, "Forbidden"},
	{ 404, "Not Found"},
	{ 408, "Request Timeout"},
	{ 410, "Gone"},
	{ 418, "I'm a teapot"}, 
	{ 429, "Too Many Requests"},

	{ 501, "Not Implemented"},		/* SERVER ERRORS */
	{ 505, "HTTP Version Not Supported"}
	// ...
};

const std::map<int, const std::string> HTTPResponseMessage::contentTypeCategories = 
{
	{ 0, "application" },
	{ 1, "audio" },
	{ 2, "image" },
	{ 3, "text" }
};

const std::string HTTPResponseMessage::format() const {
	std::string output;

	output = protocol + ' ' + std::to_string(static_cast<int>(status)) + ' ' + message + '\n'+
		"content-length: " + std::to_string(length) + '\n' +
		"content-type: " + contentTypeCategories.at(static_cast<int>(typeCat)) + '/' + typeExt + '\n' +
		// Wed, 23 Mar 2022 14:31:57 GMT
		"date: " + /* std::size_t strftime( char* str, std::size_t count, const char* format, const std::tm* time ) + */ '\n' +
		"server: " + server + '\n' +
		'\n' +
		body;

	return output;
}
