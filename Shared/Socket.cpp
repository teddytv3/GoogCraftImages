#include "Socket.h"

#include <WS2tcpip.h>

void Socket::setFail(bool newFail) {
	this->error = newFail;
}

bool Socket::initialize(WSADATA* data) {
	bool result = WSAStartup(MAKEWORD(2, 2), data);

	return result;
}

void Socket::cleanup() {
	WSACleanup();
}

Socket::Socket() {
	// Start by initializing all data to 0
	this->reset();
}

bool Socket::open() {
	bool res = false;

	this->mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	res = (this->mainSocket == INVALID_SOCKET);

	this->setFail(res);
	return res;
}

void Socket::close() {
	closesocket(this->mainSocket);
}

void Socket::reset() {
	memset(this, 0, sizeof(*this));
}

bool Socket::fail() {
	return this->error;
}

bool Socket::connect(const unsigned short int port, const char* addr) {
	bool res = false;

	// Check if the input is valid
	if (addr == nullptr) {
		res = true;

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