cmake_minimum_required(VERSION 3.17)

project(webserv)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserv main.cpp sockets/parentSocket.hpp sockets/parentSocket.cpp sockets/serverSocket.cpp sockets/serverSocket.hpp sockets/listeningSocket.cpp sockets/listeningSocket.hpp sockets/clientSocket.hpp sockets/clientSocket.cpp server/serverObject.hpp server/serverObject.cpp)
