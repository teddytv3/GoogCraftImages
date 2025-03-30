#include "Commands.h"
#include "defines.h"
#include <chrono>

namespace UI {
	const std::string parseCommand(Shared::Socket& socket, const std::string& cmdLine) {
		if (cmdLine == CUI_CMD_HELP) {
			return cmdHelp();
		}
		else if (cmdLine == CUI_CMD_UPLOAD) {
			return cmdPush(socket, cmdLine);
		}
		else if (cmdLine == CUI_CMD_DOWNLOAD) {
			return cmdPull(socket, cmdLine);
		}
		else if (cmdLine == CUI_CMD_INFO) {
			return cmdInfo(socket);
		}
		else if (cmdLine == CUI_CMD_LS) {

		}
		else if (cmdLine == CUI_CMD_MSG) {

		}
		else if (cmdLine == CUI_CMD_TELEM) {
			int preset;
			std::cout << "Select telemetry preset:\n";
			std::cout << "1. Basic Metrics\n";
			std::cout << "2. Advanced Debug Data\n";
			std::cout << "3. Full System Report\n";
			std::cout << "Enter choice: ";
			std::cin >> preset;
			std::cin.ignore(); // Clear input buffer

			// Create a packet for telemetry data
			Shared::PacketHeader hdr;
			hdr.actionID = Shared::ACT_TELEMETRY; 
			hdr.pktType = Shared::ACTION;
			hdr.sequenceNum = 0; 
			hdr.dataSize = sizeof(int);

			Shared::Packet pkt;
			pkt.setPacket(hdr, reinterpret_cast<char*>(&preset), hdr.dataSize);

			// Send the telemetry packet
			if (socket.send(pkt) < 0) {
				Shared::log("client.log", -1, "Failed to send telemetry packet to server.");
				return "Failed to send telemetry data.\n";
			}
			else {
				Shared::log("client.log", 0, "Sent telemetry packet successfully.");
				return "Telemetry data sent successfully.\n";
			}
		}

		return "Command does not exist or was typed incorrectly. Use 'help' to find existing commands.\n";
	}

	const std::string cmdHelp() {
		return
			TERM_BG_BLACK TERM_FG_WHITE " ---= GoogCraftImages - Client CLI Help Menu =---\n" TERM_RESET\
			" help\t\t\tDisplay this help menu\n"\
			" push <filename>\tUpload a file to the server\n"\
			" pull <filename>\tDownload a file from the server\n"\
			" info\t\t\tQuery the server for general information\n";
	}

	const std::string cmdPush(Shared::Socket& socket, const std::string& cmdLine) {
		return "";
	}

	const std::string cmdPull(Shared::Socket& socket, const std::string& cmdLine) {
		return "";
	}

	const std::string cmdInfo(Shared::Socket& socket) {
		// Get the current timestamp in milliseconds
		auto startTime = std::chrono::high_resolution_clock::now();
		long long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(startTime.time_since_epoch()).count();

		// Create a packet header
		Shared::PacketHeader hdr;
		hdr.actionID = Shared::ACT_INFO;
		hdr.pktType = Shared::ACTION;
		hdr.sequenceNum = 0;
		hdr.dataSize = sizeof(timestamp); // We're sending the timestamp as data

		// Create the packet and copy the timestamp into it
		Shared::Packet pkt;
		pkt.setPacket(hdr, reinterpret_cast<char*>(&timestamp), hdr.dataSize);

		// Send the packet to the server
		if (socket.send(pkt) < 0) {
			Shared::log("client.log", -1, "Failed to send info request packet to server.");
			return "Failed to request server info.\n";
		}

		// Receive the server's response
		Shared::Packet responsePkt;
		if (socket.receive(responsePkt) < 0) {
			Shared::log("client.log", -1, "Failed to receive server response.");
			return "Failed to receive server response.\n";
		}

		// Get the response time
		auto endTime = std::chrono::high_resolution_clock::now();
		double responseTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();

		// Log response time
		Shared::log("client.log", 0, "Response time: " + std::to_string(responseTime) + " ms");

		// Extract server response data
		char serverMessage[Shared::MAX_DATA_SIZE];
		memset(serverMessage, 0, Shared::MAX_DATA_SIZE);
		memcpy(serverMessage, responsePkt.getData(), responsePkt.getHeader().dataSize);

		// Return server message along with response time
		return std::string(serverMessage) + "\nResponse Time: " + std::to_string(responseTime) + " ms\n";
	}

	const std::string cmdLs() {
		return "";
	}

	const std::string cmdMsg(Shared::Socket& socket, const std::string& cmdLine) {
		return "";
	}

	const std::string cmdTelem(Shared::Socket& socket, const std::string& cmdLine) {
		return "";
	}
}