cmake_minimum_required(VERSION 3.17)

project(webserv)

set(CMAKE_CXX_STANDARD 11)

add_executable(webserv main.cpp
				sockets/parentSocket.cpp sockets/serverSocket.cpp sockets/listeningSocket.cpp sockets/clientSocket.cpp
				server/parentServer.cpp server/testServer.cpp
				http/Request.cpp
				config/parentConfig.cpp)
add_executable(catchConfig catch2/catch.hpp catch2/main.cpp catch2/configParsing.cpp config/parentConfig.cpp testers/parentConfigTester.cpp utils/stringUtils.hpp testers/fakeServer.hpp)
add_executable(parentConfigTester testers/parentConfigTester.cpp config/parentConfig.cpp)
include_directories(config)
add_subdirectory(catch2)
