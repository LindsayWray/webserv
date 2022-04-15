#pragma once

#include "socketData.hpp"
#include "clientSocket.hpp"

#include <string>

#include <arpa/inet.h>
#include <stdio.h>
// #include <string.h> 
#include <sys/socket.h>
#include <unistd.h>

/* 
	Tests:

	#1 Basic GET request, no body
	#2 Basic POST request, with content length, sent as whole

	#3 With content length, no transfer encoding, split into two chunks
	#4 With content length, no transfer encoding, split into two chunks, body too small
	#5 With content length, no transfer encoding, split into two chunks, body too large

	#6 Without content length, with transfer encoding, and transfer encoding formatting
	#7 With content length, and transfer encoding, and transfer encoding formatting
	#8 With content length, and transfer encoding, with no transfer encoding formatting
	#9 With content length, and transfer encoding, and transfer encoding formatting, last chunk missing
	#10 With content length, and transfer encoding, and transfer encoding formatting, with last chunk info too small
	#11 With content length, and transfer encoding, and transfer encoding formatting, with last chunk info too big

	#12 With content length, body longer than max body size
	#13 With transfer encoding, body longer than max body size

	#14 With URL too large
	#15 With Headers too large
*/

class RequestHandlingTest {
	public:

	void basicGETRequest(int& sock);
	void basicPOSTRequest(int& sock);

	void contentLengthSplitIntoTwoChunks(int& sock);
	void contentLengthSplitIntoTwoChunksBodyTooSmall(int& sock);
	void contentLengthSplitIntoTwoChunksBodyTooLarge(int& sock);

	void transferEncodingNoContentLength(int& sock);
	void transferEncoding(int& sock);
	void transferEncodingInvalidFormatting(void);
	void transferEncodingLastChunkMissing(void);
	void transferEncodingLastChunkTooSmall(void);
	void transferEncodingLastChunkTooBig(void);

	void contentLengthBodyLargerThanMaxBodySize(void);
	void transferEncodingBodyLargerThanMaxBodySize(void);

	void urlTooLarge(void);
	void headersTooLarge(void);

	RequestHandlingTest() {}
	~RequestHandlingTest() {}

	int setUpSocket(int sock);
	void clean(void);
};
