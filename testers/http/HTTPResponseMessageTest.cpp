#include "HTTPResponseMessageTest.hpp"
#include <iostream>

// clang++ -std=c++17 HTTPResponseMessageTest.cpp ../../srcs/http/HTTPResponseMessage.cpp -I../../srcs/http -I.

void HTTPResponseMessageTest::statusCodeMessages() {
    if (HTTPResponseMessage::responseStatusMessages.at(200) != "OK")
        throw std::exception();
    if (HTTPResponseMessage::responseStatusMessages.at(505) != "HTTP Version Not Supported")
        throw std::exception();
}

void HTTPResponseMessageTest::format() {
    std::string body = "<!doctype html><html><head><title>Our Funky HTML Page</title><meta name=\"description\" content=\"Our first page\"><meta name=\"keywords\" content=\"html tutorial template\"></head><body>Content goes here.</body></html>";
    HTTPResponseMessage sut;
    sut.addStatus(HTTPResponseMessage::OK)
        .addTypeCat(HTTPResponseMessage::TEXT)
        .addTypeExt("html")
        .addLength(body.length())
        .addBody(body);
                        
    std::cout << sut.format();
}

int main() {
    HTTPResponseMessageTest test;

    test.statusCodeMessages();
    test.format();

    return 0;
}
