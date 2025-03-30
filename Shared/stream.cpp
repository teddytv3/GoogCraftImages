#include "stream.h"
#include "defines.h"
#include "logger.h"
#include "utility.h"

#include <fstream>

namespace Shared {
	int streamOutFileOverSocket(Socket& socket, const std::string& filepath) {
		const std::string FILE_PATH = Shared::findFilePath(filepath);
		int result = 0;

		// Attempt to open the file 
		std::ifstream file = std::ifstream(FILE_PATH, std::ios::in | std::ios::binary);

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
			hdr.dataSize = filepath.length();
			pkt.setPacket(hdr, filepath.c_str(), hdr.dataSize);

			if (socket.send(pkt) < 0) {
				Shared::log("stream.log", -1, "Failed to send initial upload packet to server.");
				result = -1;
			}
			else {
				Shared::log("stream.log", 0, "Sent initial upload packet.");
			}

			// Increment sequence to indicate we are starting to send data
			hdr.sequenceNum++;

			// Send all file data to the server
			do {
				// Begin streaming file sequeneces
				file.read(data, MAX_DATA_SIZE);

				// Query the result
				std::uint16_t bytes = static_cast<uint16_t>(file.gcount());
				//std::cout << "File sequence " << hdr.sequenceNum << " was of size " << bytes << " bytes" << std::endl;

				// Send a sequence packet to the server
				hdr.dataSize = bytes;
				pkt.setPacket(hdr, data, bytes);
				if (socket.send(pkt) < 0) {
					result = -2;
					Shared::log("stream.log", result, "Failed to send upload sequence packet to server.");
				}
				else {
					Shared::log("stream.log", hdr.sequenceNum, "Sent a sequence to the server.");
				}

				hdr.sequenceNum++;

			} while (false == file.eof() && false == file.fail());

			// Send final stream packet to the server
			hdr.dataSize = 0;
			pkt.setPacket(hdr, nullptr, 0);

			if (socket.send(pkt) < 0) {
				result = -3;
				Shared::log("stream.log", result, "Failed to send final sequence packet to server.");
			}
			else {
				Shared::log("stream.log", 0, "Sent final sequence to the server.");
			}
		}
		else {
			Shared::log("stream.log", -1, "File failed to open.");
			result = -1;
		}

		return result;
	}

	int sendCustomMessage(Socket& socket, const std::vector<std::string>& message) {
		int result = 0;
		Shared::PacketHeader hdr;
		hdr.actionID = Shared::ACT_UPLOAD;
		hdr.pktType = Shared::ACTION;
		hdr.sequenceNum = 0;
		Shared::Packet pkt;

		std::string data;

		for (int i = 0; i < message.size(); i++) {
			data += message[i];
			if (i == message.size() - 1) {
				break;
			}
			data += " ";
		}

		hdr.dataSize = data.length();
		pkt.setPacket(hdr, data.c_str(), hdr.dataSize);

		if (socket.send(pkt) < 0) {
			Shared::log("stream.log", -1, "Failed to send custom message to server.");
			result = -1;
		}
		else {
			Shared::log("stream.log", 0, "Sent custom message packet.");
		}

		return result;
	}
}