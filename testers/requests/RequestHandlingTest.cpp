#include "RequestHandlingTest.hpp"

#define PORT 8080

/* COMMAND LINE

clang++ -Wall -Werror -Wextra RequestHandlingTest.cpp ../../srcs/sockets/clientSocket.cpp ../../srcs/sockets/parentSocket.cpp ../../srcs/sockets/socketData.cpp -I. -I../../srcs/sockets -o testRequestHandling && ./testRequestHandling && rm testRequestHandling

*/

void RequestHandlingTest::basicGETRequest(void) {
    std::string requestMessage =
		"GET / HTTP/1.1\r\n"\
		"host: localhost\r\n"\
		"\r\n";

    webserv::socketData sockData;
    sockData.ports.push_back(8080);

    webserv::clientSocket clientSock(sockData, 8080);
    clientSock.connect_to_network();

    int sock = clientSock.get_sock();

    send(sock, requestMessage.c_str(), strlen(requestMessage.c_str()), 0);
    std::cout << "basicGETRequest message sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "basicGETRequest done\n\n";
    return;
}

void RequestHandlingTest::basicPOSTRequest(void) {
	std::string requestMessage =
		"POST /basicPOSTRequest.txt HTTP/1.1\r\n"\
		"content-length: 24\r\n"\
		"host: localhost\r\n"\
		"\r\n"\
		"the body of this request";

    int sock = 0;

    send(sock, requestMessage.c_str(), strlen(requestMessage.c_str()), 0);
    std::cout << "basicPOSTRequest message sent\n";
    
    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "basicPOSTRequest done\n";
    
    return;
}

void RequestHandlingTest::contentLengthSplitIntoTwoChunks(void) {
	std::string firstChunk =
		"POST /contentLengthSplitIntoTwoChunksTest.txt HTTP/1.1\r\n"\
		"host: localhost\r\n"\
		"content-length: 64\r\n"\
		"\r\n"\
		"the first chunk of this request\n";

	std::string secondChunk = 
		"the second chunk of this request";

	int sock = 0;

    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    std::cout << "contentLengthSplitIntoTwoChunks: first chunk sent\n";
    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "contentLengthSplitIntoTwoChunks: second chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";
    return;
}

void RequestHandlingTest::contentLengthSplitIntoTwoChunksBodyTooSmall(void) {

}

void RequestHandlingTest::contentLengthSplitIntoTwoChunksBodyTooLarge(void) {

}


void RequestHandlingTest::transferEncoding(void) {

}

void RequestHandlingTest::transferEncodingInvalidFormatting(void) {

}

void RequestHandlingTest::transferEncodingLastChunkMissing(void) {

}

void RequestHandlingTest::transferEncodingLastChunkTooSmall(void) {

}

void RequestHandlingTest::transferEncodingLastChunkTooBig(void) {

}


void RequestHandlingTest::contentLengthBodyLargerThanMaxBodySize(void) {

}

void RequestHandlingTest::transferEncodingBodyLargerThanMaxBodySize(void) {

}


void RequestHandlingTest::urlTooLarge(void) {

}

void RequestHandlingTest::headersTooLarge(void) {

}

int main(void) {
	RequestHandlingTest sut;

    sut.basicGETRequest();
    sut.basicPOSTRequest();
	sut.contentLengthSplitIntoTwoChunks();

	return 0;
}
