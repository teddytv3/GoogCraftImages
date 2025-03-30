#include "Commands.h"
#include "defines.h"
#include "logger.h"
#include "utility.h"
#include "stream.h"

#include <fstream>

namespace UI {
	const std::string parseCommand(Shared::Socket& socket, std::vector<std::string>& args) {
		if (args.size() <= 0) {
			return "";
		}

		std::string result;
		std::string command = args[0];
		// Remove the first element
		args.erase(args.begin(), args.begin() + 1);

		if (command == CUI_CMD_HELP) {
			result = cmdHelp();
		}
		else if (command == CUI_CMD_UPLOAD) {
			result = cmdPush(socket, args);
		}
		else if (command == CUI_CMD_DOWNLOAD) {
			result = cmdPull(socket, args);
		}
		else if (command == CUI_CMD_INFO) {
			result = cmdInfo(socket);
		}
		else if (command == CUI_CMD_LS) {
			result = cmdLs();
		}
		else if (command == CUI_CMD_MSG) {
			result = cmdMsg(socket, args);
		}
		else if (command == CUI_CMD_TELEM) {
			result = cmdTelem(socket, args);
		}
		else {
			result = "Command does not exist or was typed incorrectly. Use 'help' to find existing commands.\n";
		}

		// Now return the result as a single point of exit
		return result;
	}

	const std::string cmdHelp() {
		return
			TERM_BG_BLACK TERM_FG_WHITE " ---= GoogCraftImages - Client CLI Help Menu =---\n" TERM_RESET\
			" help\t\t\tDisplay this help menu\n"\
			" push <filename>\tUpload a file to the server\n"\
			" pull <filename>\tDownload a file from the server\n"\
			" info\t\t\tQuery the server for general information\n"\
			" ls\t\t\tList files in program directories.\n"\
			" msg <message>\t\tSend a dynamically-sized message to the server.\n"\
			" telem <on/off>\t\tEnable or disable telemetry.\n";
	}

	const std::string cmdPush(Shared::Socket& socket, const std::vector<std::string>& args) {
		std::string result = "";

		int streamReturn = Shared::streamOutFileOverSocket(socket, args[0]);

		if (streamReturn == 0) {
			result = "Successfully sent file " + args[0] + " to the server!\n";
		}
		else {
			result = "Error " + std::to_string(streamReturn) + " when sending sequences.\n";
		}
		
		return result;
	}

	const std::string cmdPull(Shared::Socket& socket, const std::vector<std::string>& args) {
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
		char serverMessage[MAX_DATA_SIZE];
		memset(serverMessage, 0, MAX_DATA_SIZE);
		memcpy(serverMessage, responsePkt.getData(), responsePkt.getPacketHeader().dataSize);

		// Return server message along with response time
		return std::string(serverMessage) + "\nResponse Time: " + std::to_string(responseTime) + " ms\n";
	}

	const std::string cmdLs() {
		Shared::DisplayLogFiles();
		Shared::DisplaySentFiles();
		return "";
	}

	const std::string cmdMsg(Shared::Socket& socket, const std::vector<std::string>& args) {
		std::string result = "";

		int msgReturn = Shared::sendCustomMessage(socket, args);

		if (msgReturn == 0) {
			result = "Successfully sent custom message\n";
		}
		else {
			result = "Error sending custom message\n";
		}

		return result;
	}

	const std::string cmdTelem(Shared::Socket& socket, const std::vector<std::string>& args) {
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
}