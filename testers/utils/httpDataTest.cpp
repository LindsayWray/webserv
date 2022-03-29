#include <exception>
#include <iostream>

#include "httpDataTest.hpp"

// clang++ -std=c++17 httpDataTest.cpp -I../../srcs/utils -I. -o testHTTPData && ./testHTTPData && rm testHTTPData


void httpDataTest::findLocationBlock() {
    webserv::httpData http("/var/www/html");
    std::vector<webserv::locationData> locations;
    webserv::locationData loc;

    loc.location = "/";
    locations.push_back(loc);

    loc.location = "/resources/";
    locations.push_back(loc);

    http.locations = locations;

    if (http._findLocationBlock("/") != &(http.locations[0]))
        throw std::exception();
    if (http._findLocationBlock("/resources/") != &(http.locations[1]))
        throw std::exception();
    if (http._findLocationBlock("/troep/") != NULL)
        throw std::exception();

    if (http._findLocationBlock("/")->location != "/")
        throw std::exception();
    if (http._findLocationBlock("/resources/")->location != "/resources/")
        throw std::exception();
    
    std::cout << "findLocationBlock: TEST SUCCESS\n";
}

void httpDataTest::getReqPath() {
    webserv::httpData http("/var/www/html");
    
    std::string request = "/images/";
    if (http._getReqPath(request) != "/images/")
        throw std::exception();

    request = "/images";
    if (http._getReqPath(request) != "/images/")
        throw std::exception();

    request = "/images/2022/03/04/1400x1800.jpeg";
    if (http._getReqPath(request) != "/images/")
        throw std::exception();

    request = "/troep/hi/hi/hi";
    if (http._getReqPath(request) != "/troep/")
        throw std::exception();
    
    request = "////";
    if (http._getReqPath(request) != "/")
        throw std::exception();
    
    request = "/";
    if (http._getReqPath(request) != "/")
        throw std::exception();

    request = "/cool.html";
    if (http._getReqPath(request) != "/")
        throw std::exception();
    
    request = "/images/cool.html";
    if (http._getReqPath(request) != "/images/")
        throw std::exception();

    std::cout << "getRequestedFilePath: TEST SUCCESS\n";
}

void httpDataTest::getRequestedFilePath() {
    std::string root = "/var/www/html";
    webserv::httpData http(root);

    std::vector<webserv::locationData> locations;

    webserv::locationData loc;

    loc.location = "/images/";
    loc.root = http.abs_path + "/resources/";
    locations.push_back(loc);

    loc.location = "/imagesWithAutoIndex/";
    loc.root = http.abs_path + "/resources/";
    loc.autoindex = true;
    locations.push_back(loc);

    http.locations = locations;

    std::string request;
    std::string response;

    request = "/";
    response = http.getRequestedFilePath(request);
    if (response != "/var/www/html/index.html")
        throw std::exception();

    request = "/troep/";
    response = http.getRequestedFilePath(request);
    if (response != "/var/www/html/troep/index.html")
        throw std::exception();


    request = "/images/";
    response = http.getRequestedFilePath(request);
    if (response != "/var/www/html/resources/index.html")
        throw std::exception();

    request = "/page.html";
    response = http.getRequestedFilePath(request);
    if (response != "/var/www/html/page.html")
        throw std::exception();

    /** ADDING '/' TO LOCATIONS */
    loc.location = "/";
    loc.root = http.abs_path;
    locations.push_back(loc);

    http.locations = locations;
    /** *********************** */

    request = "/images/";
    response = http.getRequestedFilePath(request);
    if (response != "/var/www/html/resources/index.html")
        throw std::exception();

    // request = "/images";
    // response = http.getRequestedFilePath(request);
    // if (response != "/var/www/html/resources/index.html")
    //     throw std::exception();

    request = "/images/2022/03/04/1400x1800.jpeg";
    response = http.getRequestedFilePath(request);
    if (response != "/var/www/html/resources/2022/03/04/1400x1800.jpeg")
        throw std::exception();

    // request = "/image.jpeg";
    // response = http.getRequestedFilePath(request);
    // std::cout << response + "\n";

    // request = "/troep/";
    // response = http.getRequestedFilePath(request);
    // if (response != "/var/www/html/troep/index.html")
    //     throw std::exception();

    // request = "/troep";
    // response = http.getRequestedFilePath(request);
    // std::cout << response;

    // request = "/";
    // response = http.getRequestedFilePath(request);
    // if (response != "/var/www/html/index.html")
    //     throw std::exception();

    // request = "/imagesWithAutoIndex";
    // response = http.getRequestedFilePath(request);
    // std::cout << response + "\n";

    // request = "/imagesWithAutoIndex/";
    // response = http.getRequestedFilePath(request);
    // std::cout << response + "\n";

    // request = "/imagesWithAutoIndex/file.txt";
    // response = http.getRequestedFilePath(request);
    // std::cout << response;

    // request = "/imagesWithAutoIndex/dir";
    // response = http.getRequestedFilePath(request);
    // std::cout << response;

    // request = "/imagesWithAutoIndex/dir/";
    // response = http.getRequestedFilePath(request);
    // std::cout << response;

    // request = "/file.txt";
    // response = http.getRequestedFilePath(request);
    // std::cout << response;
}

int main() {
    httpDataTest test;

    test.findLocationBlock();
    test.getReqPath();
    test.getRequestedFilePath();

    return 0;
}