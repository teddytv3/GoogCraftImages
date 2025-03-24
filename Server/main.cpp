#include <iostream>
#include <string>
#include <Socket.h>
#include <defines.h>
#include "logger.h"

// Server Main
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
		log("server.log", 0, "Waiting to accept...");
		Socket client = sock.accept();
		log("server.log", 0, "Client accepted!");

		// Command accept loop
		while (!client.fail()) {
			log("server.log", 0, "Waiting for packet...");

			// Receive a single packet from the client
			Packet gotPacket;
			int receiveRes = client.receive(gotPacket);

			// If the client failed to receive, break out of this loop before we handle the bad packet
			if (client.fail()) {
				break;
			}

			// Packet received
			log("server.log", receiveRes, "Packet received! Received n bytes.");

			// Evaluate the received action
			switch (gotPacket.getPacketHeader().actionID) {
				case ActionType::ACT_DELETE:
					log("server.log", 0, "Delete action received");
					break;

				case ActionType::ACT_DOWNLOAD:
					log("server.log", 0, "Download action received");
					break;

				case ActionType::ACT_POSITION:
					log("server.log", 0, "Position action received");
					break;

				case ActionType::ACT_UPLOAD:
					log("server.log", 0, "Upload action received");
					break;

				default:
					log("server.log", 0, "Unknown action received!");
					break;
			}
		}
		// ...
		//client.send()...
	}

	Socket::cleanup();
	return 0;
}