#pragma once
#include "Socket.h"

namespace UI {
	/* @brief Perform the client UI loop until the client indicates they want to exit
	*  @param[in] client	A reference to the client socket object. Used for sending commands and receiving responses
	*/
	void doClientUILoop(Shared::Socket& client);
}