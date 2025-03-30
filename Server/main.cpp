#include <iostream>
#include <string>
#include <Socket.h>
#include <defines.h>
#include <thread>
#include "logger.h"
#include "server_interface.h"
#include "states.h"
#include "serverStream.h"

#include "utility.h"
#include <map>

// Server Main
int main(int argc, char* argv[]) {
	WSADATA data;
	Shared::Socket::initialize(&data);

	Shared::Socket sock;
	if (sock.open()) {
		Shared::log("server.log", -1, "Failed to open a socket");
		exit(1);
	}

	if (sock.serve(PORT)) {
		Shared::log("server.log", -1, "Failed to serve on the chosen port.");
		exit(1);
	}

	// State - The state of the server
	// Start in the LISTENING state
	Server::State state = Server::State::LISTENING;
	std::thread t1 = std::thread(Server::DisplayOptions);
	// Client Accept Loop
	while (state != Server::State::OFF) {
		state = Server::State::LISTENING;
		Shared::log("server.log", 0, "Transitioned to the LISTENING state...");
		Shared::Socket client = sock.accept();
		Shared::log("server.log", 0, "Client accepted!");

		// Transition to the IDLE state if we are not currently in the IDLE state
		state = Server::State::IDLE;
		Shared::log("server.log", 0, "Transitioned to the IDLE state...");

		// Buffer for dynamically allocated file sequences
		std::vector<Server::SequenceContainer> sequences;
		std::string uploadFilename = "";

		// Command accept loop
		while (!client.fail()) {
			Shared::log("server.log", 0, "Waiting for a packet...");

			// Receive a single packet from the client
			Shared::Packet gotPacket;
			int receiveRes = client.receive(gotPacket);

			// If the client failed to receive, break out of this loop before we handle the bad packet
			if (client.fail()) {
				break;
			}

			// Packet received
			Shared::log("server.log", receiveRes, "Packet received! Received n bytes.");

			// Prepare the packet response used in the response packet
			// By default, it's nack. That way we know an ACK was explicitly set
			Shared::PktType pktResponse = Shared::PktType::NACK;

			// Get a reference to the packet header
			const Shared::PacketHeader& HEADER = gotPacket.getPacketHeader();

			// Evaluate the received action
			switch (gotPacket.getPacketHeader().actionID) {
				case Shared::ActionType::ACT_DELETE:
					Shared::log("server.log", 0, "Delete action received");
					pktResponse = Shared::PktType::ACK;
					break;

				case Shared::ActionType::ACT_DOWNLOAD:
					Shared::log("server.log", 0, "Download action received");
					pktResponse = Shared::PktType::ACK;

					state = Server::State::STREAMING;
					Shared::log("server.log", 0, "Transitioned to the STREAMING state.");

					// ... Download logic
					// When the final download packet is sent, switch back to IDLE
					break;

				case Shared::ActionType::ACT_POSITION:
					Shared::log("server.log", 0, "Position action received");
					pktResponse = Shared::PktType::ACK;
					break;

				case Shared::ActionType::ACT_UPLOAD:
					Shared::log("server.log", 0, "Upload action received");
					pktResponse = Shared::PktType::ACK;


					// IF this is the first upload packet in the sequence...
					if (state != Server::State::READING) {
						if (Server::handleUploadPacketOne(state, gotPacket, uploadFilename)) {
							pktResponse = Shared::PktType::NACK;
						}
					}
					// Following data packets and final packet...
					else if (HEADER.sequenceNum > 0 && HEADER.dataSize > 0) {
						if (Server::handleUploadPacketData(gotPacket, sequences)) {
							pktResponse = Shared::PktType::NACK;
						}
					}
					else if (HEADER.sequenceNum > 0) {
						if (Server::handleUploadPacketFinal(gotPacket, sequences, state, uploadFilename)) {
							pktResponse = Shared::PktType::NACK;
						}
					}

					// ... Upload logic
					// When the final upload packet is received... switch back to IDLE
					break;

				default:
					Shared::log("server.log", 0, "Unknown action received!");
					pktResponse = Shared::PktType::NACK;
					break;
			}

			// After a command, send a response back to the client
			gotPacket.makeTelemetry(pktResponse);

			// Now send the packet back to the client
			Shared::log("server.log", client.send(gotPacket), "Sent response packet back to client.");
		}

		// Clear the sequences vector
		for (int i = 0; i < sequences.size(); i++) {
			// Free up any allocated sequences
			if (sequences[i].dataPtr != nullptr) {
				delete[] sequences[i].dataPtr;
			}
		}
		sequences.clear();
	}

	Shared::Socket::cleanup();
	t1.join();
	return 0;
}