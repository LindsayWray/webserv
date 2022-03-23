cmake_minimum_required(VERSION 3.17)

project(webserv)

set(CMAKE_CXX_STANDARD 11)

add_executable(webserv srcs/main.cpp
        srcs/sockets/parentSocket.cpp srcs/sockets/serverSocket.cpp srcs/sockets/listeningSocket.cpp srcs/sockets/clientSocket.cpp
        srcs/server/serverInstance.hpp srcs/server/serverInstance.cpp
        srcs/http/Request.cpp srcs/http/HTTPResponseMessage.cpp
        srcs/config/configParser.cpp srcs/server/accept.hpp)
add_executable(catchConfig testers/catch2/catch.hpp testers/catch2/main.cpp testers/catch2/configParsing.cpp srcs/config/configParser.cpp testers/testConfigTester/parentConfigTester.cpp srcs/utils/stringUtils.hpp testers/testConfigTester/testConfigParser.hpp srcs/utils/kqueueUtils.hpp)
add_executable(parentConfigTester testers/testConfigTester/parentConfigTester.cpp srcs/config/configParser.cpp)
include_directories(srcs/config)
add_subdirectory(testers/catch2)
