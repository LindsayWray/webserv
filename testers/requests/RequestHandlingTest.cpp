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
	std::string requestMessage =
		"POST /contentLengthBodyLargerThanMaxBodySizeTest.txt HTTP/1.1\r\n"\
		"host: localhost\r\n"\
		"content-length: 600\r\n"\
		"\r\n"\
		"the body of this request\n"\
        "[Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenea]";

    send(sock, requestMessage.c_str(), strlen(requestMessage.c_str()), 0);
    std::cout << "contentLengthBodyLargerThanMaxBodySize: request message sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "contentLengthBodyLargerThanMaxBodySize done\n\n";

    return;
}

void RequestHandlingTest::contentLengthBodyLargerThanMaxBodySizeTwoChunks(int& sock) {
	std::string firstChunk =
		"POST /contentLengthBodyLargerThanMaxBodySizeTwoChunksTest.txt HTTP/1.1\r\n"\
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
    std::string requestMessage =
        "POST /transferEncodingBodyLargerThanMaxBodySizeTest.txt HTTP/1.1\r\n"\
        "host: localhost\r\n"\
        "content-length: 64\r\n"\
        "transfer-encoding: chunked\r\n"\
        "\r\n"\
        "200\r\n"\
		"the body message of this request\n"\
        "[Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa qu]"\
        "\r\n"\
        "0\r\n"\
        "\r\n";

    send(sock, requestMessage.c_str(), strlen(requestMessage.c_str()), 0);
    std::cout << "transferEncodingBodyLargerThanMaxBodySize: first chunk sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "transferEncodingBodyLargerThanMaxBodySize done\n\n";

    return;
}

void RequestHandlingTest::transferEncodingBodyLargerThanMaxBodySizeTwoChunks(int& sock) {
    std::string firstChunk =
        "POST /transferEncodingBodyLargerThanMaxBodySizeTwoChunksTest.txt HTTP/1.1\r\n"\
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
    std::string requestMessage =
		"GET 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 HTTP/1.1\r\n"\
		"host: localhost\r\n"\
		"\r\n";

    send(sock, requestMessage.c_str(), strlen(requestMessage.c_str()), 0);
    std::cout << "urlTooLarge message sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "urlTooLarge done\n\n";
    return;
}

void RequestHandlingTest::headersTooLarge(int& sock) {
    (void)sock;
}

void RequestHandlingTest::methodNotImplemented(int& sock) {
    std::string requestMessage =
		"HEAD / HTTP/1.1\r\n"\
		"host: localhost\r\n"\
		"\r\n";

    send(sock, requestMessage.c_str(), strlen(requestMessage.c_str()), 0);
    std::cout << "methodNotImplemented message sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "methodNotImplemented done\n\n";
    return;
}

void RequestHandlingTest::versionNotImplemented(int& sock) {
    std::string requestMessage =
		"GET / HTTP/2\r\n"\
		"host: localhost\r\n"\
		"\r\n";

    send(sock, requestMessage.c_str(), strlen(requestMessage.c_str()), 0);
    std::cout << "versionNotImplemented message sent\n";

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    std::cout << buffer << "\n";

    std::cout << "versionNotImplemented done\n\n";
    return;
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
        //sut.contentLengthSplitIntoTwoChunksBodyTooSmall(sock); // should fail
        sut.contentLengthSplitIntoTwoChunksBodyTooLarge(sock); // bad request

        sut.transferEncodingNoContentLength(sock);
        sut.transferEncoding(sock);
        //sut.transferEncodingInvalidFormatting(sock); // should fail
        //sut.transferEncodingLastChunkMissing(sock); // should fail
        //sut.transferEncodingLastChunkTooSmall(sock); // should fail
        // sut.transferEncodingLastChunkTooBig(sock); // should fail

        // sut.contentLengthBodyLargerThanMaxBodySize(sock); // closes the connection
        // sut.transferEncodingBodyLargerThanMaxBodySize(sock); // closes the connection

        sut.urlTooLarge(sock);
        sut.methodNotImplemented(sock);
        sut.versionNotImplemented(sock);
    }
	return 0;
}
