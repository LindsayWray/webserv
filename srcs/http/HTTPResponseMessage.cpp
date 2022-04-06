#include "HTTPResponseMessage.hpp"

const std::map<int, const std::string> HTTPResponseMessage::responseStatusMessages =
        {
                { 200, "OK" },                        /* SUCCES */
                { 201, "Created" },
                { 202, "Accepted" },
                { 204, "No Content" },

                { 301, "Moved Permanently" },        /* REDIRECTION */
                { 302, "Found" },
                { 303, "See Other" },
                { 307, "Temporary Redirect" },

                { 400, "Bad Request" },                /* CLIENT ERRORS */
                { 403, "Forbidden" },
                { 404, "Not Found" },
                { 405, "Method Not Allowed" },
                { 408, "Request Timeout" },
                { 410, "Gone" },
                { 418, "I'm a teapot" },
                { 429, "Too Many Requests" },

                { 500, "Internal Server Error" },    /* SERVER ERRORS */
                { 501, "Not Implemented" },
                { 505, "HTTP Version Not Supported" }
                // ...
        };

const std::string HTTPResponseMessage::toString() const {
    std::string output;

    output =
            protocol + ' ' + _getStatusCodeStr() + ' ' + message + "\r\n" +

            _headerContentLengthToString() +
            _headerContentTypeToString() +
            _headerDateToString() +
            _headerLocationToString() +
            _headerServerToString() +
            "\r\n" +

            body;

    return output;
}

const std::string HTTPResponseMessage::_headerContentLengthToString( void ) const {
    if ( body != "" )
        return "content-length: " + std::to_string( length ) + "\r\n";
    else
        return "";
}

const std::string HTTPResponseMessage::_headerContentTypeToString( void ) const {
    return "content-type: " + type + "\r\n";
}

const std::string HTTPResponseMessage::_headerDateToString( void ) const {
    return "date: " + _getDateStr() + "\r\n";
}

const std::string HTTPResponseMessage::_headerLocationToString( void ) const {
    if ( location != "" )
        return "location: " + location + "\r\n";
    else
        return "";
}

const std::string HTTPResponseMessage::_headerServerToString( void ) const {
    return "server: " + server + "\r\n";
}

std::string HTTPResponseMessage::_getDateStr() const {
    auto now = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t( now );

    std::string output = std::ctime( &end_time );
    output.pop_back();    /* removes terminating newline */

    return output;
}

const std::string HTTPResponseMessage::_getStatusCodeStr() const {
    return std::to_string( static_cast<int>(status));
}
