#include <iostream>
#include <string>
#include <Socket.h>
#include <defines.h>

int main(int argc, char* argv[]) {
	WSADATA data;
	Socket::initialize(&data);

	Socket sock;
	sock.open();
	if (sock.serve(PORT)) {
		std::cout << "Failed to serve on port" << std::endl;
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