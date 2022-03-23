#include "HTTPResponseMessage.hpp"

const std::vector<const std::string> HTTPResponseMessage::responseStatusMessages = { "OK", "Created", "No Content",
													  "Moved Permanently", "Found", "See Other", "Temporary Redirect",
													  "Bad Request", "Forbidden", "Not Found", "Request Timeout", "Gone", "I'm a teapot", "Too Many Requests",
													  "Not Implemented", "HTTP Version Not Supported"
													  // ...
													  };