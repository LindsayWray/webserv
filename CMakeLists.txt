cmake_minimum_required(VERSION 3.17)

project(webserv)

set(CMAKE_CXX_STANDARD 11)

add_executable(webserv main.cpp sockets/parentSocket.hpp sockets/parentSocket.cpp sockets/serverSocket.cpp sockets/serverSocket.hpp sockets/listeningSocket.cpp sockets/listeningSocket.hpp sockets/clientSocket.hpp sockets/clientSocket.cpp server/parentServer.hpp server/parentServer.cpp utils/dataStructs.hpp server/testServer.hpp server/testServer.cpp config/parentConfig.hpp config/parentConfig.cpp)
add_executable(catchConfig catch2/catch.hpp catch2/main.cpp catch2/configParsing.cpp config/parentConfig.cpp testers/parentConfigTester.cpp)
add_executable(parentConfigTester testers/parentConfigTester.cpp config/parentConfig.cpp)
include_directories(config)
add_subdirectory(catch2)