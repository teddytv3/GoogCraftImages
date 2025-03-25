#include "Socket.h"
#include <iostream>
#include "defines.h"
#include "logger.h"

namespace Shared {
	void Socket::setFail(bool newFail) {
		this->error = newFail;
	}

	bool Socket::initialize(WSADATA* data) {
		bool result = WSAStartup(MAKEWORD(2, 2), data);

		return result;
	}

	void Socket::cleanup() {
		int cleanupRes = WSACleanup();
		if (cleanupRes == 0) {
			log("socket.log", 0, "Cleaned up.");
		}
		else {
			log("socket.log", cleanupRes, "Failed to cleanup socket");
		}
	}

	Socket::Socket() {
		// Start by initializing all data to 0
		this->reset();
	}

	bool Socket::open() {
		bool res = false;

		this->mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		res = (this->mainSocket == INVALID_SOCKET);

		if (res == true) {
			int errc = WSAGetLastError();

			std::cout << "[Socket] Failed to open socket (" << this->mainSocket << ") - " << errc << std::endl;
		}

		this->setFail(res);
		return res;
	}

	bool Socket::close() {
		int closeRes = closesocket(this->mainSocket);
		log("socket.log", closeRes, "Attempted to close socket.");

		return (closeRes != 0); // True indicates failure, false indicate success
	}

	void Socket::reset() {
		memset(this, 0, sizeof(*this));
	}

	bool Socket::fail() {
		return this->error;
	}


	int Socket::receive(Packet& packet) {
		// Dummy header to be filled during receive
		PacketHeader header;
		int32_t bytesReceived = 0;
		int32_t res = 0;

		// First read the header
		res = ::recv(this->mainSocket, ptr_cast(char, &header), sizeof(PacketHeader), 0);
		if (res < 0) {
			this->setFail(true);
			log("socket.log", -1, "Failed to receive packet header over socket");
		}
		else {
			bytesReceived = res;

			// Allocate temporary buffer for the data
			const uint32_t REMAINING_DATA_SIZE = header.dataSize + FOOTER_SIZE;
			char* dataChecksumBuf = new char[REMAINING_DATA_SIZE];
			if (dataChecksumBuf == nullptr) {
				this->setFail(true);
				log("socket.log", -2, "Failed to allocate buffer for data when receiving packet in socket");
			}
			else {
				// Now we can get the amount of data to read. We will then read that much data, plus a checksum 
				res = ::recv(this->mainSocket, dataChecksumBuf, REMAINING_DATA_SIZE, 0);
				if (res < 0) {
					this->setFail(true);
					log("socket.log", -3, "Failed to receive packet data in socket");
				}
				else {
					bytesReceived += res;
					// Now set the packet
					if (packet.setPacket(header, dataChecksumBuf, REMAINING_DATA_SIZE)) {
						this->setFail(true);
						// An error occurred when setting the packet. Set a negative response 
						log("socket.log", -4, "Failed to set the packet structure with the header, buffer, and data size");
						bytesReceived = -1;

						// I hate going this many layers deep but MISRA insists upon itself
					}
				}

				// Finally, free the data buffer (only if it wasn't nullptr)
				delete[] dataChecksumBuf;
			}
		}

		return bytesReceived;
	}

	int Socket::send(Packet const& packet) {
		int sendResult = -1; // Default to error

		// Allocate a packet to be sent
		const unsigned int PACKET_SIZE = packet.getPacketSize();
		char* buffer = new char[PACKET_SIZE];

		if (buffer == nullptr) {
			this->setFail(true);
			log("socket.log", -1, "Failed to allocate packet buffer during send");
		}
		else {

			// Serialize the packet into the new buffer
			packet.serialize(buffer, PACKET_SIZE);

			// Send the buffer over the socket
			sendResult = ::send(this->mainSocket, buffer, PACKET_SIZE, 0);

			// Free up the packet buffer before we even check if the sent was successful or not
			// Do this as soon as possible
			delete[] buffer;

			// Check if an error occurred so we can log it
			// We still want to return the return value that ::send proves
			//
			// An error is determined if the number of bytes sent is not equal to the packet size
			if (static_cast<long int>(sendResult) != static_cast<long int>(PACKET_SIZE)) {
				log("socket.log", sendResult, "Failed to send the correct number of bytes");
				this->setFail(true);
			}
		}

		return sendResult;
	}

	bool Socket::connect(const unsigned short int port, const char* addr) {
		bool res = false;

		// Check if the input is valid
		if (addr == nullptr) {
			res = true;

			std::cout << "[Socket] Failed to connect to NULL address" << std::endl;
			this->setFail(res);
			return res;
		}

		// Now create the address structure
		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		//serverAddr.sin_addr.s_addr = inet_addr(addr);
		inet_pton(AF_INET, addr, &serverAddr.sin_addr.s_addr);

		// Now attempt to connect
		res = (SOCKET_ERROR == ::connect(this->mainSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)));

		if (res == true) {
			std::cout << "[Socket] Failed to connect to address" << std::endl;
		}

		this->setFail(res);
		return res;
	}

	bool Socket::serve(const unsigned short int port) {
		bool res = false;

		// Create the address
		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_port = htons(port);

		res = (SOCKET_ERROR == ::bind(this->mainSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)));

		// If successfull...
		if (false == res) {
			// Set the socket as a listening socket
			res = (SOCKET_ERROR == listen(this->mainSocket, 1));
			if (res == true) {
				log("socket.log", -1, "Socket failed to listen");
			}
		}
		else {
			log("socket.log", -1, "Socket failed to bind");
		}

		this->setFail(res);
		return res;

	}

	Socket Socket::accept() {
		Socket clientSock;

		// Attempt to accept a client connection
		SOCKET client = ::accept(this->mainSocket, nullptr, nullptr);

		// Check if it was an error
		if (SOCKET_ERROR == client) {
			log("socket.log", -1, "Socket failed to accept");
			this->setFail(true);
			clientSock.setFail(true);
		}
		else {
			// If successful, set the returned socket's value
			clientSock.mainSocket = client;
		}

		// Return the new socket
		return clientSock;
	}
}