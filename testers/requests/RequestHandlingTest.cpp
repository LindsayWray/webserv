#include "RequestHandlingTest.hpp"

#define PORT 8080

/* COMMAND LINE
Run from root/testers/requests/:
clang++ -Wall -Werror -Wextra RequestHandlingTest.cpp ../../srcs/sockets/clientSocket.cpp ../../srcs/sockets/parentSocket.cpp ../../srcs/sockets/socketData.cpp -I. -I../../srcs/sockets -o testRequestHandling && ./testRequestHandling && rm testRequestHandling

Run from root:
clang++ -Wall -Werror -Wextra testers/requests/RequestHandlingTest.cpp srcs/sockets/clientSocket.cpp srcs/sockets/parentSocket.cpp srcs/sockets/socketData.cpp -Itesters/requests -Isrcs/sockets -o testRequestHandling && ./testRequestHandling && rm testRequestHandling

Remove test output files: 
clang++ -Wall -Werror -Wextra testers/requests/RequestHandlingTest.cpp srcs/sockets/clientSocket.cpp srcs/sockets/parentSocket.cpp srcs/sockets/socketData.cpp -Itesters/requests -Isrcs/sockets -o testRequestHandling && ./testRequestHandling clean && rm testRequestHandling

*/

void RequestHandlingTest::basicGETRequest(int& sock) {
    std::string requestMessage =
		"GET / HTTP/1.1\r\n"\
		"host: localhost\r\n"\
		"\r\n";

    send(sock, requestMessage.c_str(), strlen(requestMessage.c_str()), 0);
    std::cout << "basicGETRequest message sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "basicGETRequest done\n\n";
    return;
}

void RequestHandlingTest::basicPOSTRequest(int& sock) {
	std::string requestMessage =
		"POST /basicPOSTRequest.txt HTTP/1.1\r\n"\
		"content-length: 24\r\n"\
		"host: localhost\r\n"\
		"\r\n"\
		"the body of this request";

    send(sock, requestMessage.c_str(), strlen(requestMessage.c_str()), 0);
    std::cout << "basicPOSTRequest message sent\n";
    
    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "basicPOSTRequest done\n";
    
    return;
}

void RequestHandlingTest::contentLengthSplitIntoTwoChunks(int& sock) {
	std::string firstChunk =
		"POST /contentLengthSplitIntoTwoChunksTest.txt HTTP/1.1\r\n"\
		"host: localhost\r\n"\
		"content-length: 64\r\n"\
		"\r\n"\
		"the first chunk of this request\n";

	std::string secondChunk = 
		"the second chunk of this request";

    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    std::cout << "contentLengthSplitIntoTwoChunks: first chunk sent\n";

    sleep(2);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "contentLengthSplitIntoTwoChunks: second chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    return;
}

void RequestHandlingTest::contentLengthSplitIntoTwoChunksBodyTooSmall(int& sock) {
    std::string firstChunk =
        "POST /contentLengthSplitIntoTwoChunksBodyTooSmallTest.txt HTTP/1.1\r\n"\
        "host: localhost\r\n"\
        "content-Length: 64\r\n"\
        "\r\n"\
        "the first chunk of this request\n";

	std::string secondChunk = 
		"the second chu...";

    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    std::cout << "contentLengthSplitIntoTwoChunksBodyTooSmall: first chunk sent\n";

    sleep(2);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "contentLengthSplitIntoTwoChunksBodyTooSmall: second chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    return;
}

void RequestHandlingTest::contentLengthSplitIntoTwoChunksBodyTooLarge(int& sock) {
    std::string firstChunk =
        "POST /contentLengthSplitIntoTwoChunksBodyTooLargeTest.txt HTTP/1.1\r\n"\
        "host: localhost\r\n"\
        "content-length: 64\r\n"\
        "\r\n"\
        "the first chunk of this request\n";

	std::string secondChunk = 
		"the second chunk of this request is too large";

    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    std::cout << "contentLengthSplitIntoTwoChunksBodyTooLarge: first chunk sent\n";

    sleep(2);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "contentLengthSplitIntoTwoChunksBodyTooLarge: second chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    return;
}


void RequestHandlingTest::transferEncodingNoContentLength(int& sock) {
    std::string firstChunk =
        "POST /transferEncodingNoContentLengthTest.txt HTTP/1.1\r\n"\
        "host: localhost\r\n"\
        "transfer-encoding: chunked\r\n"\
        "\r\n"\
        "20\r\n"\
        "the first chunk of this request\n\r\n";

    std::string secondChunk =
        "20\r\n"\
		"the second chunk of this request\r\n";

    std::string terminatingChunk =
        "0\r\n"\
        "\r\n";

    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    std::cout << "transferEncodingNoContentLength: first chunk sent\n";

    sleep(2);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "transferEncodingNoContentLength: second chunk sent\n";

    sleep(2);

    send(sock, terminatingChunk.c_str(), strlen(terminatingChunk.c_str()), 0);
    std::cout << "transferEncodingNoContentLength: terminating chunk sent\n";


    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    return;
}

void RequestHandlingTest::transferEncoding(int& sock) {
    std::string firstChunk =
        "POST /transferEncodingTest.txt HTTP/1.1\r\n"\
        "host: localhost\r\n"\
        "content-length: 64\r\n"\
        "transfer-encoding: chunked\r\n"\
        "\r\n"\
        "20\r\n"\
        "the first chunk of this request\n\r\n";

    std::string secondChunk =
        "20\r\n"\
		"the second chunk of this request\r\n";

    std::string terminatingChunk =
        "0\r\n"\
        "\r\n";

    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    std::cout << "transferEncoding: first chunk sent\n";

    sleep(2);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "transferEncoding: second chunk sent\n";

    sleep(2);

    send(sock, terminatingChunk.c_str(), strlen(terminatingChunk.c_str()), 0);
    std::cout << "transferEncoding: terminating chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    return;
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

int RequestHandlingTest::setUpSocket(int port) {
    webserv::socketData sockData;
    sockData.ports.push_back(port);

    webserv::clientSocket clientSock(sockData, port);
    clientSock.connect_to_network();

    return clientSock.get_sock();
}

void RequestHandlingTest::clean(void) {
    std::remove( "var/www/html/basicPOSTRequest.txt" );
    std::remove( "var/www/html/transferEncodingNoContentLengthTest.txt" );
    std::remove( "var/www/html/transferEncodingTest.txt" );

    std::cout << "Cleaned previously generated test files\n\n";
}

int main(int argc, char** argv) {
	RequestHandlingTest sut;
    sut.clean();

    if (argc == 1 || std::strcmp(argv[1], "clean") != 0) {
        int sock = sut.setUpSocket(8080);

        sut.basicGETRequest(sock);
        sut.basicPOSTRequest(sock);

        // sut.contentLengthSplitIntoTwoChunks(sock);
        // // sut.contentLengthSplitIntoTwoChunksBodyTooSmall(sock);
        // sut.contentLengthSplitIntoTwoChunksBodyTooLarge(sock);

        sut.transferEncodingNoContentLength(sock);
        sut.transferEncoding(sock);
    }
	return 0;
}
