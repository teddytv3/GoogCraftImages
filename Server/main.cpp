#include <iostream>
#include <string>
#include <Socket.h>
#include <defines.h>
#include <thread>
#include "logger.h"
#include "server_interface.h"
#include "states.h"

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

					state = Server::State::READING;
					Shared::log("server.log", 0, "Transitioned to the READING state.");

					// ... Upload logic
					// When the final upload packet is received... switch back to IDLE
					break;
				case Shared::ActionType::ACT_INFO:
					Shared::log("server.log", 0, "Info action received");

					// Extract timestamp from the packet
					long long clientTimestamp = 0;
					memcpy(&clientTimestamp, gotPacket.getData(), sizeof(long long));

					auto startTime = std::chrono::high_resolution_clock::now();

					std::string serverInfo = "Server Info: Version 1.0.0, Uptime: 24h";

					// Create the response packet with the server info
					Shared::PacketHeader hdr;
					hdr.actionID = Shared::ActionType::ACT_INFO;
					hdr.pktType = Shared::PktType::ACK; // ACK response
					hdr.sequenceNum = 0;
					hdr.dataSize = serverInfo.length() + 1; // Include null terminator

					Shared::Packet responsePkt;
					responsePkt.setPacket(hdr, serverInfo.c_str(), hdr.dataSize);

					// Send the response packet back to the client
					if (client.send(responsePkt) < 0) {
						Shared::log("server.log", -1, "Failed to send server response packet.");
						break;
					}

					// Calculate the response time
					auto endTime = std::chrono::high_resolution_clock::now();
					double responseTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();

					// Log the server response time
					Shared::log("server.log", 0, "Response time: " + std::to_string(responseTime) + " ms");

					pktResponse = Shared::PktType::ACK; // Set response type to ACK
					break;
				case Shared::ActionType::ACT_TELEMETRY:
					Shared::log("server.log", 0, "Telemetry action received");

					// Extract telemetry data from the packet
					std::string telemetryData = reinterpret_cast<const char*>(gotPacket.getData());

					Shared::log("server.log", 0, "Received telemetry: " + telemetryData);


					// Create an ACK response packet for telemetry
					Shared::PacketHeader hdr;
					hdr.actionID = Shared::ActionType::ACT_TELEMETRY; 
					hdr.pktType = Shared::PktType::ACK; 
					hdr.sequenceNum = 0;
					hdr.dataSize = 0; // No data in the response packet

					Shared::Packet responsePkt;
					responsePkt.setPacket(hdr, nullptr, hdr.dataSize);

					if (client.send(responsePkt) < 0) {
						Shared::log("server.log", -1, "Failed to send telemetry response packet.");
						break;
					}
					pktResponse = Shared::PktType::ACK; 
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
	}

	Shared::Socket::cleanup();
	t1.join();
	return 0;
}