#include "pch.h"
#include "CppUnitTest.h"

#include "../Client/Commands.h"
#include "../Server/serverStream.h"
#include "../Shared/Packet.h"
#include <utility.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(Client)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			Assert::AreEqual(1, 1);
		}
	};

	TEST_CLASS(Server)
	{
	public:

		TEST_METHOD(UPLOAD_001)
		{
			Shared::Packet packet;
			Shared::PacketHeader header;
			header.actionID = Shared::ActionType::ACT_UPLOAD;
			header.sequenceNum = 3;
			header.pktType = Shared::ACTION;
			header.dataSize = 0;

			Assert::AreEqual(false, packet.setPacket(header, nullptr, 0));

			::Server::State state = ::Server::State::READING;
			std::string filename = "";

			// Ensure that the packet errors
			Assert::AreEqual(true, ::Server::handleUploadPacketOne(state, packet, filename));
		}

		TEST_METHOD(UPLOAD_002)
		{
			const std::string PACKET_FILENAME = "test.txt";
			const std::string EXPECTED_FILENAME = Shared::getRootPath() + "files/DL-" + PACKET_FILENAME;

			Shared::Packet packet;
			Shared::PacketHeader header;
			header.actionID = Shared::ActionType::ACT_UPLOAD;
			header.sequenceNum = 0;
			header.pktType = Shared::ACTION;
			header.dataSize = PACKET_FILENAME.length();

			Assert::AreEqual(false, packet.setPacket(header, PACKET_FILENAME.c_str(), PACKET_FILENAME.length()));

			::Server::State state = ::Server::State::READING;
			std::string filename = "";

			// Ensure that the packet does not contain an error
			Assert::AreEqual(false, ::Server::handleUploadPacketOne(state, packet, filename));

			// Compare the filenames
			Assert::AreEqual(EXPECTED_FILENAME, filename);
		}
		

		TEST_METHOD(UPLOAD_003)
		{
			const int TOTAL_SEQUENCES = 4;
			const std::string SEQUENCES[] = {
				"ONE",
				"TWO",
				"THREE",
				"FOUR"
			};

			Shared::Packet packet;
			Shared::PacketHeader header;
			header.actionID = Shared::ActionType::ACT_UPLOAD;
			header.pktType = Shared::ACTION;

			// Vector of saved sequences
			std::vector<::Server::SequenceContainer> sequences;

			// For each sequence.. handle as packet
			for (int i = 0; i < TOTAL_SEQUENCES; i++) {
				header.sequenceNum = i + 1;
				header.dataSize = SEQUENCES[i].length();
				Assert::AreEqual(false, packet.setPacket(header, SEQUENCES[i].c_str(), header.dataSize));

				::Server::State state = ::Server::State::READING;
				std::string filename = "";

				// Ensure that the packet does not contain an error
				Assert::AreEqual(false, ::Server::handleUploadPacketData(packet, sequences));
			}

			// Now, check that the received sequences match what was expected, in the same order
			for (int i = 0; i < TOTAL_SEQUENCES; i++) {
				Assert::AreEqual(0, ::memcmp(SEQUENCES[i].c_str(), sequences[i].dataPtr, sequences[i].bytes));
				Assert::AreEqual(SEQUENCES[i].length(), static_cast<size_t>(sequences[i].bytes));
			}
		}

		TEST_METHOD(UPLOAD_004)
		{
			const int TOTAL_SEQUENCES = 4;
			const std::string SEQUENCES[] = {
				"ONE",
				"TWO",
				"THREE",
				"FOUR"
			};

			::Server::State state = ::Server::State::READING;
			Shared::Packet packet;
			Shared::PacketHeader header;
			header.actionID = Shared::ActionType::ACT_UPLOAD;
			header.pktType = Shared::ACTION;

			std::string UPLOAD_FILENAME = "UPLOAD_004";

			// Vector of saved sequences
			std::vector<::Server::SequenceContainer> sequences;

			// For each sequence.. handle as packet
			for (int i = 0; i < TOTAL_SEQUENCES; i+=2) {
				header.sequenceNum = i + 1;
				header.dataSize = SEQUENCES[i].length();
				Assert::AreEqual(false, packet.setPacket(header, SEQUENCES[i].c_str(), header.dataSize));

				::Server::State state = ::Server::State::READING;
				std::string filename = "";

				// Ensure that the packet does not contain an error
				Assert::AreEqual(false, ::Server::handleUploadPacketData(packet, sequences));
			}

			Assert::AreEqual(true, ::Server::handleUploadPacketFinal(packet, sequences, state, UPLOAD_FILENAME));
		}

		TEST_METHOD(UPLOAD_005)
		{
			const int TOTAL_SEQUENCES = 4;
			const std::string SEQUENCES[] = {
				"ONE",
				"TWO",
				"THREE",
				"FOUR"
			};

			::Server::State state = ::Server::State::READING;
			Shared::Packet packet;
			Shared::PacketHeader header;
			header.actionID = Shared::ActionType::ACT_UPLOAD;
			header.pktType = Shared::ACTION;

			std::string UPLOAD_FILENAME = Shared::getRootPath() + "files/UPLOAD_005.txt";

			// Vector of saved sequences
			std::vector<::Server::SequenceContainer> sequences;

			// For each sequence.. handle as packet
			for (int i = 0; i < TOTAL_SEQUENCES; i++) {
				header.sequenceNum = i + 1;
				header.dataSize = SEQUENCES[i].length();
				Assert::AreEqual(false, packet.setPacket(header, SEQUENCES[i].c_str(), header.dataSize));

				::Server::State state = ::Server::State::READING;
				std::string filename = "";

				// Ensure that the packet does not contain an error
				Assert::AreEqual(false, ::Server::handleUploadPacketData(packet, sequences));
			}

			Assert::AreEqual(false, ::Server::handleUploadPacketFinal(packet, sequences, state, UPLOAD_FILENAME));
		}
	};
}
