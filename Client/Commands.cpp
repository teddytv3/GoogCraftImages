#include "Commands.h"
#include "defines.h"
#include "logger.h"
#include "utility.h"

#include <fstream>

namespace UI {
	const std::string parseCommand(Shared::Socket& socket, std::vector<std::string>& args) {
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
		const std::string FILE_PATH = Shared::findFilePath(args[0]);
		std::string result = "";

		// Attempt to open the file 
		std::ifstream file = std::ifstream(FILE_PATH, std::ios::in);
		
		// Ensure the file opens properly
		if (!file.fail() && !file.bad()) {
			// Sequence buffer for file data
			char data[MAX_DATA_SIZE];

			// Packet to be sent
			Shared::PacketHeader hdr;
			hdr.actionID = Shared::ACT_UPLOAD;
			hdr.pktType = Shared::ACTION;
			hdr.sequenceNum = 0;
			Shared::Packet pkt;

			// Send the initial packet with the filename as data
			hdr.dataSize = args[0].length();
			pkt.setPacket(hdr, args[0].c_str(), hdr.dataSize);

			if (socket.send(pkt) < 0) {
				Shared::log("client.log", -1, "Failed to send initial upload packet to server.");
			}
			else {
				Shared::log("client.log", 0, "Sent initial upload packet.");	
			}

			// Increment sequence to indicate we are starting to send data
			hdr.sequenceNum++;

			// Send all file data to the server
			do {
				// Begin streaming file sequeneces
				file.read(data, MAX_DATA_SIZE);

				// Query the result
				uint16_t bytes = static_cast<uint16_t>(file.gcount());

				// Send a sequence packet to the server
				hdr.dataSize = bytes;
				pkt.setPacket(hdr, data, bytes);
				if (socket.send(pkt) < 0) {
					Shared::log("client.log", -1, "Failed to send upload sequence packet to server.");
				}
				else {
					Shared::log("client.log", 0, "Sent a sequence to the server.");
				}

				hdr.sequenceNum++;

			} while (false == file.eof() && false == file.fail());
			
			// Send final stream packet to the server
			hdr.dataSize = 0;
			pkt.setPacket(hdr, nullptr, 0);

		}
		else {
			result = "Error: Could not find file.";
		}

		return result;
	}

	const std::string cmdPull(Shared::Socket& socket, const std::vector<std::string>& args) {
		return "";
	}

	const std::string cmdInfo(Shared::Socket& socket) {
		return "";
	}

	const std::string cmdLs() {
		Shared::DisplayLogFiles();
		Shared::DisplaySentFiles();
		return "";
	}

	const std::string cmdMsg(Shared::Socket& socket, const std::vector<std::string>& args) {
		return "";
	}

	const std::string cmdTelem(Shared::Socket& socket, const std::vector<std::string>& args) {
		return "";
	}
}