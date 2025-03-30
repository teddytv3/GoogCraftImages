#pragma once
#include "string"
#include "Socket.h"

namespace Shared {
	/* @brief Send a file over a socket
	*  @param[in] socket	A reference to the open socket that the file will be sent over
	*  @param[in] path		A full static path to the file that will be sent
	*  @return				A status code. <0 if any packets failed to send. 0 otherwise.
	*/
	extern int streamOutFileOverSocket(Socket& socket, const std::string& filepath);
}