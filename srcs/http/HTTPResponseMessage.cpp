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

const std::string HTTPResponseMessage::toString() const {
	std::string output;

	output =
		protocol + ' ' + _getStatusCodeStr() + ' ' + message + "\r\n" +

		"content-length: " + std::to_string(length) + "\r\n" +
		"content-type: " + _getTypeCatStr() + '/' + typeExt + "\r\n" +
		"date: " + _getDateStr() + "\r\n" +
		"server: " + server + "\r\n" +
		"\r\n" +

		body;

	return output;
}

std::string HTTPResponseMessage::_getDateStr() const {
	auto now = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(now);
	
	std::string output = std::ctime(&end_time);
	output.pop_back();

    return output;
}

const std::string HTTPResponseMessage::_getStatusCodeStr() const {
	return std::to_string(static_cast<int>(status));
}

const std::string HTTPResponseMessage::_getTypeCatStr() const {
	return HTTPResponseMessage::contentTypeCategories.at(static_cast<int>(typeCat));
}
