#include <iostream>
#include <string>
#include <Socket.h>
#include <defines.h>

int main(int argc, char* argv[]) {
	WSADATA data;
	Socket::initialize(&data);

	Socket sock;
	if (sock.open()) {
		exit(1);
	}

	if (sock.serve(PORT)) {
		exit(1);
	}
	
	while (true) {
		printf("Waiting \n");
		Socket client = sock.accept();

		// ...
		// client.send()...
	}

	return 0;
}