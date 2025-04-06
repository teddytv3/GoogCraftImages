#include "pch.h"
#include "CppUnitTest.h"

#include "../Client/Commands.h"
#include <defines.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(Packet)
	{
	public:

		TEST_METHOD(PACKET_001)
		{
			// Initial Data
			const char INPUT_DATA[] = { 1, 3, 5, 2, 99 };
			const unsigned short int INPUT_SIZE = sizeof(INPUT_DATA) / sizeof(char);

			// Create the packet, attempt to copy the data
			Shared::Packet packet;
			bool copyRes = packet.copyData(INPUT_DATA, INPUT_SIZE);

			// Ensure there was no error.
			Assert::AreEqual(copyRes, false);

			// Ensure the pointers are different, and the data matches
			Assert::AreNotEqual(INPUT_DATA, packet.getData());

			// Ensure the data within the pointers matches
			Assert::AreEqual(::memcmp(INPUT_DATA, packet.getData(), INPUT_SIZE), 0);
		}

		TEST_METHOD(PACKET_002)
		{
			// Initial Data
			const char* INPUT_DATA = nullptr;
			const unsigned short int INPUT_SIZE = 100;
			// Set a large size (100) to ensure that this value is not placed in the header
			//  if the data buffer was null.

			// Create the packet, attempt to copy the data
			Shared::Packet packet;
			bool copyRes = packet.copyData(INPUT_DATA, INPUT_SIZE);
			// Ensure there was no error. (null data is still accepted, it will recalculate CRC)
			Assert::AreEqual(copyRes, false);
			
			// Ensure the newly constructed packet has a null pointer - no data was allocated
			Assert::AreEqual(nullptr, packet.getData());
			
			// Ensure the size was not copied into the packet header
			Assert::AreEqual(packet.getPacketHeader().dataSize, static_cast<uint16_t>(0));
		}

		TEST_METHOD(PACKET_004_HEADER)
		{
			// Initial Data
			const char* INPUT_DATA = nullptr;
			const unsigned short int INPUT_SIZE = 0;
			// Initial packet header with no data
			const Shared::PacketHeader header = {
				.actionID = Shared::ActionType::ACT_INFO,
				.pktType = Shared::PktType::ACTION,
				.sequenceNum = 5632,
				.dataSize = INPUT_SIZE,
			};

			// Header Bytes:
			// 0x05 0x00 0x16 0x00 0x00 0x00
			//
			// Expected Chekcsum:
			// 0x05 ^ 0x00 ^ 0x16 ^ 0x00 ^ 0x00 ^ 0x00
			// 0x05 ^ 0x16
			// 0x05 = 0b00000101
			// 0x16 = 0b00010110
			// Result:
			// 0x13 = 0b00010011
			const uint8_t EXPECTED_HEADER_CC = 0x13;

			// Create the packet, attempt to copy the data
			Shared::Packet packet;
			bool setRes = packet.setPacket(header, INPUT_DATA, INPUT_SIZE);
			// Ensure there was no error. (null data is still accepted, it will recalculate CRC)
			Assert::AreEqual(setRes, false);

			// Compare the CRC computed on the header
			Assert::AreEqual(packet.getChecksum(), EXPECTED_HEADER_CC);
		}

		TEST_METHOD(PACKET_004_DATA)
		{
			// Initial Data
			const char INPUT_DATA[] = { 1, 3, 5, 7, 2 };
			const unsigned short int INPUT_SIZE = sizeof(INPUT_DATA);
			// Initial packet header with no data
			const Shared::PacketHeader header = {
				.actionID = Shared::ActionType::ACT_UPLOAD,
				.pktType = Shared::PktType::ACTION,
				.sequenceNum = 0,
				.dataSize = INPUT_SIZE,
			};

			// Header Bytes:
			// 0x00 0x00 0x00 0x00 0x00 0x05
			// Data Bytes:
			// 0x01 0x03 0x05 0x07 0x02
			//
			// Expected Chekcsum:
			// 0x00 ^ 0x00 ^ 0x00 ^ 0x00 ^ 0x00 ^ 0x05 ^ 0x01 ^ 0x03 ^ 0x05 ^ 0x07 ^ 0x02
			// 0x05 ^ 0x01 ^ 0x03 ^ 0x05 ^ 0x07 ^ 0x02
			// 0x05 = 0b0101
			// 0x01 = 0b0001
			// 0x03 = 0b0011
			// 0x05 = 0b0101
			// 0x07 = 0b0111
			// 0x02 = 0b0010
			// Result:
			// 0x07 = 0b0111
			const uint8_t EXPECTED_HEADER_CC = 0x07;

			// Create the packet, attempt to copy the data
			Shared::Packet packet;
			bool setRes = packet.setPacket(header, INPUT_DATA, INPUT_SIZE);
			// Ensure there was no error. (null data is still accepted, it will recalculate CRC)
			Assert::AreEqual(setRes, false);

			// Compare the CRC computed on the header
			Assert::AreEqual(packet.getChecksum(), EXPECTED_HEADER_CC);
		}

		TEST_METHOD(PACKET_005)
		{
			// Initial Data
			const char INPUT_DATA[] = { 1, 3, 5, 2, 99, 4, 6, 1, 34 };
			const unsigned short int INPUT_SIZE = sizeof(INPUT_DATA) / sizeof(char);

			// Create the packet, attempt to copy the data
			Shared::Packet packet;
			bool copyRes = packet.copyData(INPUT_DATA, INPUT_SIZE);

			// Ensure there was no error.
			Assert::AreEqual(copyRes, false);

			// Ensure the data within the pointers matches
			Assert::AreEqual(packet.getPacketSize(), static_cast<unsigned int>(HEADER_SIZE + INPUT_SIZE + FOOTER_SIZE));
		}

		TEST_METHOD(PACKET_006)
		{
			// Initial Data
			const char INPUT_DATA[] = { 1, 55, 5, 2, 99, 4, 6, 1, 34 };
			const unsigned short int INPUT_SIZE = sizeof(INPUT_DATA) / sizeof(char);
			const char INITIAL_HEADER[] = { 0, 0, 0, 0, INPUT_SIZE, 0 };

			// Create the packet, attempt to copy the data
			Shared::Packet packet;
			bool copyRes = packet.copyData(INPUT_DATA, INPUT_SIZE);

			// Ensure there was no error.
			Assert::AreEqual(copyRes, false);

			const unsigned int SERIALIZED_SIZE = packet.getPacketSize();
			char* serializeBuffer = new char[SERIALIZED_SIZE];
			Assert::AreNotEqual(serializeBuffer, nullptr);

			// Serialize into the buffer
			bool serialRes = packet.serialize(serializeBuffer, SERIALIZED_SIZE);
			Assert::AreEqual(serialRes, false);

			// Compare header within serialize buffer
			Assert::AreEqual(::memcmp(INITIAL_HEADER, serializeBuffer, HEADER_SIZE), 0);
			serializeBuffer += HEADER_SIZE;
			Assert::AreEqual(::memcmp(INPUT_DATA, serializeBuffer, INPUT_SIZE), 0);
			serializeBuffer += INPUT_SIZE;
			Assert::AreEqual(static_cast<uint8_t>(*serializeBuffer), packet.getChecksum());
		}

		TEST_METHOD(PACKET_007)
		{
			// Initial Data
			const char INPUT_DATA[] = { 1, 55, 5, 2, 99, 4, 6, 1, 34 };
			const unsigned short int INPUT_SIZE = sizeof(INPUT_DATA) / sizeof(char);
			const char INITIAL_HEADER[] = { 0, 0, 0, 0, INPUT_SIZE, 0 };

			// Create the packet, attempt to copy the data
			Shared::Packet packet;
			bool copyRes = packet.copyData(INPUT_DATA, INPUT_SIZE);

			// Ensure there was no error.
			Assert::AreEqual(copyRes, false);

			const unsigned int SERIALIZED_SIZE = packet.getPacketSize();
			char* serializeBuffer = nullptr;

			// Serialize into the buffer
			bool serialRes = packet.serialize(serializeBuffer, SERIALIZED_SIZE);
			Assert::AreEqual(serialRes, true);
		}

		TEST_METHOD(PACKET_008)
		{
			// Initial Data
			const char INPUT_DATA[] = { 1, 55, 5, 2, 99, 4, 6, 1, 34 };
			const unsigned short int INPUT_SIZE = sizeof(INPUT_DATA) / sizeof(char);
			const Shared::PacketHeader HEADER = {
				.actionID = Shared::ActionType::ACT_MESSAGE,
				.pktType = Shared::PktType::ACTION,
				.sequenceNum = 5,
				.dataSize = INPUT_SIZE,
			};
			const Shared::PktType NEW_PACKET_TYPE = Shared::ACK;
			const uint8_t EXPECTED_TELEM_CHECKSUM = 0x05; // HEADER.actionID^ NEW_PACKET_TYPE;

			// Create the packet, attempt to copy the data
			Shared::Packet packet;
			bool copyRes = packet.setPacket(HEADER, INPUT_DATA, INPUT_SIZE);

			// Ensure there was no error.
			Assert::AreEqual(copyRes, false);

			// Ensure the data pointer was set
			Assert::AreNotEqual(packet.getData(), nullptr);
			
			// Convert the packet into telemetry
			packet.makeTelemetry(NEW_PACKET_TYPE);

			// Compare the packet header and data
			Assert::AreEqual(static_cast<uint8_t>(HEADER.actionID), static_cast<uint8_t>(packet.getPacketHeader().actionID));
			Assert::AreEqual(static_cast<uint16_t>(0), packet.getPacketHeader().dataSize);
			Assert::AreEqual(static_cast<uint8_t>(NEW_PACKET_TYPE), static_cast<uint8_t>(packet.getPacketHeader().pktType));
			Assert::AreEqual(static_cast<uint16_t>(0), packet.getPacketHeader().sequenceNum);
			Assert::AreEqual(nullptr, packet.getData());
			Assert::AreEqual(EXPECTED_TELEM_CHECKSUM, packet.getChecksum());
		}

		TEST_METHOD(PACKET_009)
		{
			// Initial Data
			const char* INPUT_DATA = nullptr;
			const unsigned short int INPUT_SIZE = 0;
			const Shared::PacketHeader HEADER = {
				.actionID = Shared::ActionType::ACT_MESSAGE,
				.pktType = Shared::PktType::ACTION,
				.sequenceNum = 5,
				.dataSize = INPUT_SIZE,
			};

			// Create the packet, attempt to copy the data
			Shared::Packet packet;
			bool copyRes = packet.setPacket(HEADER, INPUT_DATA, INPUT_SIZE);

			// Ensure there was no error.
			Assert::AreEqual(false, copyRes);

			// Ensure the packet headers are equal
			Assert::AreEqual(::memcmp(&HEADER, &packet.getPacketHeader(), sizeof(Shared::PacketHeader)), 0);
		}

		TEST_METHOD(PACKET_010)
		{
			// Initial Data
			const char INPUT_DATA[] = { 1, 55, 5, 2, 99, 4, 6, 1, 34 };
			const unsigned short int INPUT_SIZE = sizeof(INPUT_DATA) / sizeof(char);
			const Shared::PacketHeader HEADER = {
				.actionID = Shared::ActionType::ACT_MESSAGE,
				.pktType = Shared::PktType::ACTION,
				.sequenceNum = 5,
				.dataSize = INPUT_SIZE,
			};

			// Create the packet, attempt to copy the data
			Shared::Packet packet;
			bool copyRes = packet.setPacket(HEADER, INPUT_DATA, INPUT_SIZE);

			// Ensure there was no error.
			Assert::AreEqual(false, copyRes);

			// Ensure the data pointer was set
			Assert::AreNotEqual(nullptr, packet.getData());

			// Convert the packet into telemetry
			Assert::AreNotEqual(INPUT_DATA, packet.getData());

			// Compare the data within the buffers
			Assert::AreEqual(::memcmp(INPUT_DATA, packet.getData(), INPUT_SIZE), 0);
		}

		TEST_METHOD(PACKET_011)
		{
			// Initial Data - Includes checksum
			const uint8_t EXPECTED_CHECKSUM = 0xFF;
			const char INPUT_DATA[] = { 1, 2, 3, EXPECTED_CHECKSUM };
			const unsigned short int INPUT_SIZE = sizeof(INPUT_DATA);
			const Shared::PacketHeader HEADER = {
				.actionID = Shared::ActionType::ACT_MESSAGE,
				.pktType = Shared::PktType::ACTION,
				.sequenceNum = 5,
				.dataSize = INPUT_SIZE - 1,
			};

			// Create the packet, attempt to copy the data
			Shared::Packet packet;
			bool copyRes = packet.setPacket(HEADER, INPUT_DATA, INPUT_SIZE);

			// Ensure there was no error.
			Assert::AreEqual(false, copyRes);

			// Ensure the data pointer was set
			Assert::AreNotEqual(nullptr, packet.getData());

			// Convert the packet into telemetry
			Assert::AreNotEqual(INPUT_DATA, packet.getData());

			// Compare the data within the buffers
			Assert::AreEqual(::memcmp(INPUT_DATA, packet.getData(), HEADER.dataSize), 0);

			// Checksum was copied, not recalculated
			Assert::AreEqual(EXPECTED_CHECKSUM, packet.getChecksum());
		}
	};
}
