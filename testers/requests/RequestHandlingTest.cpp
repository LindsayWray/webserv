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

    std::cout << "basicPOSTRequest done\n\n";
    
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

    sleep(1);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "contentLengthSplitIntoTwoChunks: second chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "contentLengthSplitIntoTwoChunks done\n\n";

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

    sleep(1);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "contentLengthSplitIntoTwoChunksBodyTooSmall: second chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "contentLengthSplitIntoTwoChunksBodyTooSmall done\n\n";

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

    sleep(1);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "contentLengthSplitIntoTwoChunksBodyTooLarge: second chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "contentLengthSplitIntoTwoChunksBodyTooLarge done\n\n";

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

    sleep(1);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "transferEncodingNoContentLength: second chunk sent\n";

    sleep(1);

    send(sock, terminatingChunk.c_str(), strlen(terminatingChunk.c_str()), 0);
    std::cout << "transferEncodingNoContentLength: terminating chunk sent\n";


    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "transferEncodingNoContentLength done\n\n";

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

    sleep(1);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "transferEncoding: second chunk sent\n";

    sleep(1);

    send(sock, terminatingChunk.c_str(), strlen(terminatingChunk.c_str()), 0);
    std::cout << "transferEncoding: terminating chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "transferEncoding done\n\n";

    return;
}

void RequestHandlingTest::transferEncodingInvalidFormatting(int& sock) {
    std::string firstChunk =
        "POST /transferEncodingInvalidFormattingTest.txt HTTP/1.1\r\n"\
        "host: localhost\r\n"\
        "content-length: 64\r\n"\
        "transfer-encoding: chunked\r\n"\
        "\r\n"\
        "the first chunk of this request\n\r\n";

    std::string secondChunk =
		"the second chunk of this request\r\n";

    std::string terminatingChunk =
        "\r\n";

    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    std::cout << "transferEncodingInvalidFormatting: first chunk sent\n";

    sleep(1);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "transferEncodingInvalidFormatting: second chunk sent\n";

    sleep(1);

    send(sock, terminatingChunk.c_str(), strlen(terminatingChunk.c_str()), 0);
    std::cout << "transferEncodingInvalidFormatting: terminating chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "transferEncodingInvalidFormatting done\n\n";

    return;
}

void RequestHandlingTest::transferEncodingLastChunkMissing(int& sock) {
    std::string firstChunk =
        "POST /transferEncodingLastChunkMissingTest.txt HTTP/1.1\r\n"\
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
        "";

    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    std::cout << "transferEncodingLastChunkMissing: first chunk sent\n";

    sleep(1);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "transferEncodingLastChunkMissing: second chunk sent\n";

    sleep(1);

    send(sock, terminatingChunk.c_str(), strlen(terminatingChunk.c_str()), 0);
    std::cout << "transferEncodingLastChunkMissing: empty terminating chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "transferEncodingLastChunkMissing done\n\n";

    return;
}

void RequestHandlingTest::transferEncodingLastChunkTooSmall(int& sock) {
    std::string firstChunk =
        "POST /transferEncodingLastChunkTooSmallTest.txt HTTP/1.1\r\n"\
        "host: localhost\r\n"\
        "content-length: 64\r\n"\
        "transfer-encoding: chunked\r\n"\
        "\r\n"\
        "20\r\n"\
        "the first chunk of this request\n\r\n";

    std::string secondChunk =
        "40\r\n"\
		"the second chunk of this request\r\n";

    std::string terminatingChunk =
        "0\r\n"\
        "\r\n";

    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    std::cout << "transferEncodingLastChunkTooSmall: first chunk sent\n";

    sleep(1);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "transferEncodingLastChunkTooSmall: second chunk sent\n";

    sleep(1);

    send(sock, terminatingChunk.c_str(), strlen(terminatingChunk.c_str()), 0);
    std::cout << "transferEncodingLastChunkTooSmall: terminating chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "transferEncodingLastChunkTooSmall done\n\n";

    return;
}

void RequestHandlingTest::transferEncodingLastChunkTooBig(int& sock) {
    std::string firstChunk =
        "POST /transferEncodingLastChunkTooBigTest.txt HTTP/1.1\r\n"\
        "host: localhost\r\n"\
        "content-length: 64\r\n"\
        "transfer-encoding: chunked\r\n"\
        "\r\n"\
        "20\r\n"\
        "the first chunk of this request\n\r\n";

    std::string secondChunk =
        "2\r\n"\
		"the second chunk of this request\r\n";

    std::string terminatingChunk =
        "0\r\n"\
        "\r\n";

    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    std::cout << "transferEncodingLastChunkTooBig: first chunk sent\n";

    sleep(1);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "transferEncodingLastChunkTooBig: second chunk sent\n";

    sleep(1);

    send(sock, terminatingChunk.c_str(), strlen(terminatingChunk.c_str()), 0);
    std::cout << "transferEncodingLastChunkTooBig: terminating chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "transferEncodingLastChunkTooBig done\n\n";

    return;
}


void RequestHandlingTest::contentLengthBodyLargerThanMaxBodySize(int& sock) {
	std::string firstChunk =
		"POST /contentLengthBodyLargerThanMaxBodySizeTest.txt HTTP/1.1\r\n"\
		"host: localhost\r\n"\
		"content-length: 640\r\n"\
		"\r\n"\
		"the first chunk of this request\n";

	std::string secondChunk = 
		"the second chunk of this request\n"\
        "[Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenea]";

    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    std::cout << "contentLengthBodyLargerThanMaxBodySize: first chunk sent\n";

    sleep(1);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "contentLengthBodyLargerThanMaxBodySize: second chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "contentLengthBodyLargerThanMaxBodySize done\n\n";

    return;
}

void RequestHandlingTest::transferEncodingBodyLargerThanMaxBodySize(int& sock) {
    std::string firstChunk =
        "POST /transferEncodingBodyLargerThanMaxBodySizeTest.txt HTTP/1.1\r\n"\
        "host: localhost\r\n"\
        "content-length: 64\r\n"\
        "transfer-encoding: chunked\r\n"\
        "\r\n"\
        "20\r\n"\
        "the first chunk of this request\n\r\n";

    std::string secondChunk =
        "200\r\n"\
		"the second chunk of this request\n"\
        "[Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa qu]"\
        "\r\n";

    std::string terminatingChunk =
        "0\r\n"\
        "\r\n";

    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    std::cout << "transferEncodingBodyLargerThanMaxBodySize: first chunk sent\n";

    sleep(1);

    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    std::cout << "transferEncodingBodyLargerThanMaxBodySize: second chunk sent\n";

    sleep(1);

    send(sock, terminatingChunk.c_str(), strlen(terminatingChunk.c_str()), 0);
    std::cout << "transferEncodingBodyLargerThanMaxBodySize: terminating chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "transferEncodingBodyLargerThanMaxBodySize done\n\n";

    return;
}


void RequestHandlingTest::urlTooLarge(int& sock) {
    (void)sock;
}

void RequestHandlingTest::headersTooLarge(int& sock) {
    (void)sock;
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

    std::remove( "var/www/html/contentLengthSplitIntoTwoChunksTest.txt" );
    std::remove( "var/www/html/contentLengthSplitIntoTwoChunksBodyTooSmallTest.txt" );
    std::remove( "var/www/html/contentLengthSplitIntoTwoChunksBodyTooLargeTest.txt" );

    std::remove( "var/www/html/transferEncodingNoContentLengthTest.txt" );
    std::remove( "var/www/html/transferEncodingTest.txt" );
    std::remove( "var/www/html/transferEncodingInvalidFormattingTest.txt" );
    std::remove( "var/www/html/transferEncodingLastChunkMissingTest.txt" );
    std::remove( "var/www/html/transferEncodingLastChunkTooSmallTest.txt" );
    std::remove( "var/www/html/transferEncodingLastChunkTooBigTest.txt" );

    std::remove( "var/www/html/contentLengthBodyLargerThanMaxBodySizeTest.txt" );
    std::remove( "var/www/html/transferEncodingBodyLargerThanMaxBodySizeTest.txt" );

    std::cout << "Cleaned previously generated test files\n\n";
}

int main(int argc, char** argv) {
	RequestHandlingTest sut;
    sut.clean();

    if (argc == 1 || std::strcmp(argv[1], "clean") != 0) {
        int sock = sut.setUpSocket(8080);

        sut.basicGETRequest(sock);
        sut.basicPOSTRequest(sock);

        sut.contentLengthSplitIntoTwoChunks(sock);
        //sut.contentLengthSplitIntoTwoChunksBodyTooSmall(sock); // causes the webserver to crash upon sequential requests
        sut.contentLengthSplitIntoTwoChunksBodyTooLarge(sock); // segfaults the webserver

        sut.transferEncodingNoContentLength(sock);
        sut.transferEncoding(sock);
        //sut.transferEncodingInvalidFormatting(sock); // creates file and returns OK causes sequential requests to return a 405
        //sut.transferEncodingLastChunkMissing(sock); // causes the webserver to segfault upon sequential requests
        //sut.transferEncodingLastChunkTooSmall(sock); // causes the webserver to segfault upon sequential requests
        // sut.transferEncodingLastChunkTooBig(sock); // creates file and returns OK but causes sequential requests to return a 405

        sut.contentLengthBodyLargerThanMaxBodySize(sock);
        sut.transferEncodingBodyLargerThanMaxBodySize(sock); // hangs 
    }
	return 0;
}
