cmake_minimum_required(VERSION 3.17)

project(webserv)

set(CMAKE_CXX_STANDARD 11)

add_executable(webserv srcs/main.cpp
        srcs/sockets/parentSocket.cpp srcs/sockets/serverSocket.cpp srcs/sockets/listeningSocket.cpp srcs/sockets/clientSocket.cpp
		srcs/server/handler.cpp srcs/server/CGI.cpp srcs/server/responder.cpp srcs/server/accepter.cpp srcs/server/server.hpp
		srcs/http/Request.cpp srcs/http/HTTPResponseMessage.cpp srcs/http/HTTPResponseMessageContentTypeMap.cpp
        srcs/config/configParser.cpp)
add_executable(parentConfigTester testers/testConfigTester/parentConfigTester.cpp srcs/config/configParser.cpp)
include_directories(srcs/config)
add_subdirectory(testers/catch2)
