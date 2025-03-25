#include <iostream>
#include <string>
#include <Socket.h>
#include <defines.h>
#include "logger.h"

#include "states.h"

using namespace Shared;

// Server Main
int main(int argc, char* argv[]) {
	WSADATA data;
	Socket::initialize(&data);

	Socket sock;
	if (sock.open()) {
		log("server.log", -1, "Failed to open a socket");
		exit(1);
	}

	if (sock.serve(PORT)) {
		log("server.log", -1, "Failed to serve on the chosen port.");
		exit(1);
	}

	// State - The state of the server
	// Start in the LISTENING state
	Server::State state = Server::State::LISTENING;
	
	// Client Accept Loop
	while (state != Server::State::OFF) {
		state = Server::State::LISTENING;
		log("server.log", 0, "Transitioned to the LISTENING state...");
		Socket client = sock.accept();
		log("server.log", 0, "Client accepted!");

		// Transition to the IDLE state if we are not currently in the IDLE state
		state = Server::State::IDLE;
		log("server.log", 0, "Transitioned to the IDLE state...");

		// Command accept loop
		while (!client.fail()) {
			log("server.log", 0, "Waiting for a packet...");

			// Receive a single packet from the client
			Packet gotPacket;
			int receiveRes = client.receive(gotPacket);

			// If the client failed to receive, break out of this loop before we handle the bad packet
			if (client.fail()) {
				break;
			}

			// Packet received
			log("server.log", receiveRes, "Packet received! Received n bytes.");

			// Prepare the packet response used in the response packet
			// By default, it's nack. That way we know an ACK was explicitly set
			PktType pktResponse = PktType::NACK;

			// Evaluate the received action
			switch (gotPacket.getPacketHeader().actionID) {
				case ActionType::ACT_DELETE:
					log("server.log", 0, "Delete action received");
					pktResponse = PktType::ACK;
					break;

				case ActionType::ACT_DOWNLOAD:
					log("server.log", 0, "Download action received");
					pktResponse = PktType::ACK;

					state = Server::State::STREAMING;
					log("server.log", 0, "Transitioned to the STREAMING state.");

					// ... Download logic
					// When the final download packet is sent, switch back to IDLE
					break;

				case ActionType::ACT_POSITION:
					log("server.log", 0, "Position action received");
					pktResponse = PktType::ACK;
					break;

				case ActionType::ACT_UPLOAD:
					log("server.log", 0, "Upload action received");
					pktResponse = PktType::ACK;

					state = Server::State::READING;
					log("server.log", 0, "Transitioned to the READING state.");

					// ... Upload logic
					// When the final upload packet is received... switch back to IDLE
					break;

				default:
					log("server.log", 0, "Unknown action received!");
					pktResponse = PktType::NACK;
					break;
			}

			// After a command, send a response back to the client
			gotPacket.makeTelemetry(pktResponse);

			// Now send the packet back to the client
			log("server.log", client.send(gotPacket), "Sent response packet back to client.");
		}
	}

	Socket::cleanup();
	return 0;
}