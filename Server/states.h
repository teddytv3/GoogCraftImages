#pragma once

namespace Server {
	enum State {
		// The server has been shut down
		OFF,

		// The server is waiting for a client to connect
		LISTENING,

		// The server is connected to a client. It is waiting for the client to send a command.
		IDLE,

		// The server is streaming file sequences to the client
		STREAMING,

		// The server is reading file sequences from the client
		READING
	};
}