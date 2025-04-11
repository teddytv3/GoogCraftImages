#include "pch.h"
#include "CppUnitTest.h"

#include "../Client/Commands.h"
#include <defines.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(Socket)
	{
	public:

		TEST_METHOD(SOCKET_001)
		{
			Shared::Socket sock;
			sock.open();
			sock.connect(2000, "2.3.3.3");
			
			// A failure should be raised 
			Assert::AreEqual(true, sock.fail());

			sock.close();
		}

		TEST_METHOD(SOCKET_002)
		{
			Shared::Socket server;
			Shared::Socket client;

			// Open an instance of a socket for the server and client
			server.open();
			client.open();

			// Attempt to connect to the server before the server has started. This should result in failure.
			bool connectRes = client.connect(PORT, "127.0.0.1");
			Assert::AreEqual(true, connectRes);

			// Start a TCP server on the application port.
			server.serve(PORT);
			// Connect to the started TCP server. We can do this in the same thread since the operating system takes care of the server in another thread
			// We can access the connection later on in an 'accept()' call.
			connectRes = client.connect(PORT, "127.0.0.1");

			// Ensure the connection result was false indicating no error
			Assert::AreEqual(false, connectRes);

			// Close the server and client
			server.close();
			client.close();
		}

		TEST_METHOD(SOCKET_003)
		{
			Shared::Socket server;
			Shared::Socket client;

			// Prepare a packet object to be sent 
			const std::string TEST_DATA_BUFFER = "This is some data to be sent as a test";
			const Shared::Packet IN_PACKET(Shared::ActionType::ACT_INFO, Shared::ACTION, 0, TEST_DATA_BUFFER.length(), TEST_DATA_BUFFER.c_str());
			const int PKT_SIZE = IN_PACKET.getPacketSize();

			// Open an instance of a socket for the server and client
			server.open();
			client.open();

			// Create a server on the application port, then attempt to connect to the server 
			server.serve(PORT);
			bool connectRes = client.connect(PORT, "127.0.0.1");

			// Accept a socket to use for communicating with 'client' 
			Shared::Socket accepted = server.accept();

			// Send the packet to the server, compare the number of bytes sent to confirm it was successful
			const int SENT_SIZE = client.send(IN_PACKET);
			Assert::AreEqual(PKT_SIZE, SENT_SIZE);

			// Receive the packet
			Shared::Packet outPacket;
			const int RECV_SIZE = accepted.receive(outPacket);
			Assert::AreEqual(PKT_SIZE, RECV_SIZE);

			// Compare the header of the packet
			Assert::AreEqual(0, ::memcmp(&IN_PACKET.getPacketHeader(), &outPacket.getPacketHeader(), sizeof(Shared::PacketHeader)));
			// Compare the data field of the packet
			Assert::AreEqual(0, ::memcmp(outPacket.getData(), TEST_DATA_BUFFER.c_str(), outPacket.getPacketHeader().dataSize));
			// Compare the CRC
			Assert::AreEqual(IN_PACKET.getChecksum(), outPacket.getChecksum());

			// Close all the sockets
			accepted.close();
			server.close();
			client.close();
		}
		TEST_METHOD(SOCKET_004)
		{
			Shared::Socket server;
			Shared::Socket client;

			// Prepare a packet object to be sent 
			const std::string TEST_DATA_BUFFER = "This is some data to be sent as a test";
			const Shared::Packet IN_PACKET(Shared::ActionType::ACT_INFO, Shared::ACTION, 0, TEST_DATA_BUFFER.length(), TEST_DATA_BUFFER.c_str());
			const int PKT_SIZE = IN_PACKET.getPacketSize();

			// Open an instance of a socket for the server and client
			server.open();
			client.open();

			// Create a server on the application port, then attempt to connect to the server 
			server.serve(PORT);
			bool connectRes = client.connect(PORT, "127.0.0.1");

			// Accept a socket to use for communicating with 'client' 
			Shared::Socket accepted = server.accept();

			// Close the connection gracefully
			client.close();

			// Receive the packet
			Shared::Packet outPacket;
			const int RECV_SIZE = accepted.receive(outPacket);
			// 0 return size indicates a successful disconnection
			Assert::AreEqual(0, RECV_SIZE);

			accepted.close();
			accepted.reset();

			// Now, we want to detect a failure when the server-side disconnects
			client.open();
			Assert::AreEqual(false, client.connect(PORT, "127.0.0.1"));

			accepted = server.accept();

			Assert::AreEqual(PKT_SIZE, client.send(IN_PACKET));
			Assert::AreEqual(PKT_SIZE, accepted.receive(outPacket));

			// Induce a failure by disconnecting the accepted-side of the connection. 
			accepted.close();

			accepted.receive(outPacket);

			Assert::AreEqual(true, accepted.fail());


			// Close all the sockets
			server.close();
		}
	};
}
