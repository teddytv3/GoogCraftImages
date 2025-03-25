#include <iostream>
#include <Socket.h>
#include <defines.h>
#include "logger.h"

using namespace Shared;

// Client Main
int main(int argc, char* argv[]) {
	WSADATA data;
	Socket::initialize(&data);

	Socket client;
	client.open();

	bool connectRes = client.connect(PORT, "127.0.0.1");
	log("client.log", connectRes, "Attempted to connect to client.");

	// Prepare a transmission to begin uploading a file
	std::string filename = "file1_test.txt";
	Packet packet(ActionType::ACT_UPLOAD, PktType::ACTION, 0, filename.length(), filename.c_str());

	// Send the packet to the client
	int bytes = client.send(packet);
	log("client.log", bytes, "Sent packet to server. Sent n bytes.");

	bytes = client.receive(packet);
	log("client.log", bytes, "Received response from server. Received n bytes.");


	Socket::cleanup();
	return 0;
}