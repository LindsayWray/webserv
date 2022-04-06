cmake_minimum_required(VERSION 3.17)

project(webserv)

set(CMAKE_CXX_STANDARD 11)

add_executable(webserv srcs/main.cpp
		srcs/utils/webserv.cpp srcs/utils/webservInit.cpp srcs/utils/dataStructs.cpp
		srcs/sockets/parentSocket.cpp srcs/sockets/serverSocket.cpp srcs/sockets/listeningSocket.cpp srcs/sockets/clientSocket.cpp
		srcs/server/handler.cpp srcs/server/CGI.cpp srcs/server/responder.cpp srcs/server/accepter.cpp srcs/server/autoindexing.cpp
		srcs/http/Request.cpp srcs/http/HTTPResponseMessage.cpp srcs/http/HTTPResponseMessageContentTypeMap.cpp
        srcs/config/configParser.cpp srcs/config/configParserSetters.cpp srcs/config/configParserLocation.cpp srcs/config/configParserUtils.cpp )
add_executable(catchConfig testers/catch2/catch.hpp testers/catch2/main.cpp testers/catch2/configParsing.cpp srcs/config/configParser.cpp 
		testers/testConfigTester/parentConfigTester.cpp testers/testConfigTester/testConfigParser.hpp srcs/utils/printFormatting.hpp srcs/config/configParserLocation.cpp srcs/config/configParserUtils.cpp srcs/config/configParserSetters.cpp srcs/utils/webservInit.cpp srcs/utils/dataStructs.cpp srcs/utils/webserv.hpp srcs/utils/webserv.cpp)
#add_executable(testers/testConfigTester/parentConfigTester.cpp srcs/config/configParser.cpp)
include_directories(srcs/config)
add_subdirectory(testers/catch2)
