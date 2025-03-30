#include "serverStream.h"

#include "logger.h"
#include "utility.h"

namespace Server {
	bool handleUploadPacketOne(Server::State& state, Shared::Packet const& gotPacket, std::string& uploadFilename) {
		bool error = false;

		if (gotPacket.getPacketHeader().sequenceNum == 0) {
			state = Server::State::READING;
			Shared::log("server.log", 0, "Transitioned to the READING state.");

			// Safely copy out the buffer
			char nameBuffer[200];
			::memset(nameBuffer, 0, sizeof(nameBuffer));
			::memcpy(nameBuffer, gotPacket.getData(), (gotPacket.getPacketHeader().dataSize < 200) ? gotPacket.getPacketHeader().dataSize : 199);

			// First packet with filename
			uploadFilename = "DL-" + std::string(nameBuffer);
			uploadFilename = Shared::findFilePath(uploadFilename);
			std::cout << "Preparing file upload for '" << uploadFilename << "'. Waiting for sequences..." << std::endl;
		}
		else {
			Shared::log("server.log", -1, "Server was not in reading state, but sequence was greater than 0. We are missing the new filename.");
			error = true;
		}

		return error;
	}

	bool handleUploadPacketData(Shared::Packet const& gotPacket, std::vector<SequenceContainer>& sequences) {
		bool error = false;
		const Shared::PacketHeader& HEADER = gotPacket.getPacketHeader();

		// Resize sequences vector to fit total sequence count
		if (HEADER.sequenceNum > sequences.size()) {
			// Resize the vector to fit all values, initialize each index with nullptr
			sequences.resize(HEADER.sequenceNum, { nullptr, 0 });
		}

		// Allocate space for the sequence at the correct index. 
		sequences[HEADER.sequenceNum - 1].dataPtr = new char[HEADER.dataSize];
		if (sequences[HEADER.sequenceNum - 1].dataPtr == nullptr) {
			Shared::log("server.log", -1, "Failed to allocate memory for upload sequence.");
			error = true;
		}
		else {
			// Copy the packet data into the sequence buffer
			memcpy(sequences[HEADER.sequenceNum - 1].dataPtr, gotPacket.getData(), HEADER.dataSize);
			// Save the size of the sequence
			sequences[HEADER.sequenceNum - 1].bytes = HEADER.dataSize;
		}

		return error;
	}

	bool handleUploadPacketFinal(Shared::Packet const& gotPacket, std::vector<SequenceContainer>& sequences, Server::State& state, std::string& uploadFilename) {
		bool error = false;

		// Header.dataSize == 0
		// This is the final sequence
		// Make sure all sequences prior to this sequence number are present
		bool allPresent = true;
		for (uint16_t i = 0; i < sequences.size(); i++) {
			if (sequences[i].dataPtr == nullptr) {
				Shared::log("server.log", i + 1, "Sequence number was missing. Final packet came out of order, or a packet was dropped.");
				allPresent = false;
			}
		}

		// All sequences present, write to file
		if (allPresent) {
			// Open file for writing
			std::ofstream file(uploadFilename, std::ios::out | std::ios::binary);
			if (file.fail() || file.bad()) {
				Shared::log("server.log", -1, "Failed to open file for writing.");
				error = true;
			}
			else {
				for (uint16_t i = 0; i < sequences.size(); i++) {
					file.write(sequences[i].dataPtr, sequences[i].bytes);

					if (file.fail() || file.bad()) {
						Shared::log("server.log", -1, "Failed to write to disk. Unexpected error. File went bad.");
						error = true;
						break; // Break out of the for loop
					}
				}

				Shared::log("server.log", 0, "Done writing file to disk.");

				// Close the file
				file.close();
			}
		}
		// Sequences were missing. Respond with failure.
		else {
			error = true;
		}

		// Clear the sequences vector
		for (int i = 0; i < sequences.size(); i++) {
			// Free up any allocated sequences
			if (sequences[i].dataPtr != nullptr) {
				delete[] sequences[i].dataPtr;
			}
		}
		sequences.clear();

		state = Server::State::LISTENING;
		Shared::log("server.log", 0, "Transitioned to the LISTENING state.");

		return error;
	}
}