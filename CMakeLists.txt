cmake_minimum_required(VERSION 3.17)

project(webserv)

set(CMAKE_CXX_STANDARD 11)

add_executable(webserv
		srcs/http/Request.cpp srcs/http/HTTPResponseMessage.cpp srcs/http/HTTPResponseMessageContentTypeMap.cpp
		srcs/sockets/parentSocket.cpp srcs/sockets/serverSocket.cpp srcs/sockets/listeningSocket.cpp srcs/sockets/clientSocket.cpp srcs/sockets/socketData.cpp
		srcs/utils/dataStructs.cpp
        srcs/config/configParser.cpp srcs/config/configParserSetters.cpp srcs/config/configParserLocation.cpp srcs/config/configParserUtils.cpp
		srcs/handler/handler.cpp srcs/handler/autoindexing.cpp srcs/handler/handlerUtils.cpp
        srcs/CGI/CGIResponse.cpp srcs/CGI/CGIRegister.cpp
		srcs/webserver/webserv.cpp srcs/webserver/webservInit.cpp srcs/webserver/accepter.cpp srcs/webserver/responder.cpp
		srcs/main.cpp)
#add_executable(catchConfig testers/catch2/catch.hpp testers/catch2/main.cpp testers/catch2/configParsing.cpp srcs/config/configParser.cpp
#		testers/testConfigTester/parentConfigTester.cpp testers/testConfigTester/testConfigParser.hpp srcs/utils/printFormatting.hpp srcs/config/configParserLocation.cpp srcs/config/configParserUtils.cpp srcs/config/configParserSetters.cpp srcs/webserv/webservInit.cpp srcs/utils/dataStructs.cpp srcs/webserv/webserv.hpp srcs/webserv/webserv.cpp)
#add_executable(testers/testConfigTester/parentConfigTester.cpp srcs/config/configParser.cpp)
include_directories(srcs/config srcs/CGI srcs/handler srcs/http srcs/sockets srcs/utils srcs/webserver)
add_subdirectory(testers/catch2)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
