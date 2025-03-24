#include <iostream>
#include <string>
#include <Socket.h>
#include <defines.h>
#include "logger.h"

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
	
	// Client Accept Loop
	while (true) {
		log("server.log", 0, "Waiting...");
		Socket client = sock.accept();

		// Command accept loop
		while (true) {
			Packet gotPacket;
			client.receive(gotPacket);
		}
		// ...
		//client.send()...
	}

	Socket::cleanup();
	return 0;
}