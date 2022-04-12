#include "RequestHandlingTest.hpp"

#define PORT 8080

// clang++ -Wall -Werror -Wextra RequestHandlingTest.cpp -o testRequestHandling && ./testRequestHandling && rm testRequestHandling

int RequestHandlingTest::_setUpSocket(int& sock) {
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
 
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }
 
    if (connect(sock, (struct sockaddr*)&serv_addr,
                sizeof(serv_addr))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    return 0;
}

void RequestHandlingTest::basicGETRequest(void) {
    std::string requestMessage =
		"GET / HTTP/1.1\r\n\
		host: localhost\r\n\
		\r\n";

    int sock = 0;

    if (_setUpSocket(sock) == -1)
        return;

    send(sock, requestMessage.c_str(), strlen(requestMessage.c_str()), 0);
    printf("basicGETRequest message sent\n");

    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);

    return;

}

void RequestHandlingTest::basicPOSTRequest(void) {
	std::string requestMessage =
		"POST /basicPOSTRequest.txt HTTP/1.1\r\n\
		host: localhost\r\n\
		content-length: 24\r\n\
		\r\n\
		the body of this request";

    int sock = 0;

    send(sock, requestMessage.c_str(), strlen(requestMessage.c_str()), 0);
    printf("basicPOSTRequest message sent\n");
    
    int valread;
    char buffer[1024] = { 0 };
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);

    return;
}

void RequestHandlingTest::contentLengthSplitIntoTwoChunks(void) {
	std::string firstChunk =
		"POST /contentLengthSplitIntoTwoChunksTest.txt HTTP/1.1\r\n\
		host: localhost\r\n\
		content-length: 64\r\n\
		\r\n\
		the first chunk of this request\n";

	std::string secondChunk = 
		"the second chunk of this request";

	int sock = 0, valread;
    struct sockaddr_in serv_addr;
    // char* hello = "Hello from client";
    char buffer[1024] = { 0 };
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
 
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return;
    }
 
    if (connect(sock, (struct sockaddr*)&serv_addr,
                sizeof(serv_addr))
        < 0) {
        printf("\nConnection Failed \n");
        return;
    }
    send(sock, firstChunk.c_str(), strlen(firstChunk.c_str()), 0);
    send(sock, secondChunk.c_str(), strlen(secondChunk.c_str()), 0);
    printf("contentLengthSplitIntoTwoChunks message sent\n");
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);
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
    // sut.basicPOSTRequest();
	// sut.contentLengthSplitIntoTwoChunks();

	return 0;
}
