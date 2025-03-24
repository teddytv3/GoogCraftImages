#pragma once

// Winsock headers
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600 //-V2573
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "ws2_32.lib")

#include "Packet.h"

class Socket {
private:
	SOCKET mainSocket;

	bool error;

	/* @brief Set whether this socket is in a failed state or not.
	*  @param[in] newFail	True if in the new failed state, false otherwise
	*/
	void setFail(bool newFail);

public:
	/* @brief Call WSAStartup. This should only be done once at the start of the program
	*  @param[out] data	A pointer to an empty WSADATA variable for WSA Data created by WSAStartup
	*  @return			True if failure, false otherwise
	*/
	static bool initialize(WSADATA* data);

	/* @brief Call WSACleanup. This should only be run when the program is terminating. 
	*/
	static void cleanup();

	/* @brief Create a TCP socket from scratch
	*/
	Socket();

	/* @brief Open a new socket
	*/
	bool open();

	/* @brief Close the socket
	*/
	void close();

	/* @brief Reset all variables to 0.
	*/
	void reset();

	/* @brief Return true if the socket failed the last operation, false otherwise. 
	*  @return			True if in failed state, false otherwise
	*/
	bool fail();

	/* @brief Receive a packet from a socket
	*  @param[out] packet	A reference to an empty packet object that will be populated with the received packet
	*  @return				The number of bytes that were received, <0 upon error
	*/
	int receive(Packet& packet);

	/* @brief Send a packet over a connected socket
	*  @param[in] packet	A reference to a populated packet that will be sent
	*  @return				The number of bytes that were sent. <0 upon error
	*/
	int send(Packet const& packet);

	/*
	*  ---- CLIENT UTILITIES ----
	*/

	/* @brief Connect to some specified server over TCP
	*  @param[in] port	A 2-byte value representing the port of the destination.
	*  @param[in] addr	A null-terminated string representing the IP address to connect to
	*  @return			True upon failure, false otherwise. Equal return value to .fail()
	*/
	bool connect(const unsigned short int port, const char* addr);

	/*
	*  ---- SERVER UTILITIES ----
	*/

	/* @brief Bind a socket to a port listening to any 0.0.0.0, then set the port as a listening socket.
	*  @param[in] port	The port to listen on as a server
	*  @return			True upon failure, false otherwise. Equal return value to .fail()
	*/
	bool serve(const unsigned short int port);

	/* @brief Accept a new client connection as a separate socket object
	*  @return	A new socket object containing the connected socket. The new socket object can have the 'fail' bit set if an error occurred during accept. 
	*/
	Socket accept();

};