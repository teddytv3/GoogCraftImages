#include "ClientUI.h"

#include "Commands.h"
#include "utility.h"
#include "defines.h"
#include <iostream>
#include <sstream>

namespace UI {
	void doClientUILoop(Shared::Socket& client) {
		//char commandBuffer[CUI_MAX_COMMAND_LENGTH];
		std::string commandBuffer = "";
		std::ostringstream outputStream;

		while (true) {
			// CLean reusable buffers
			commandBuffer = "";
			outputStream.str("");

			// Draw the prompt
			outputStream << "ClientUI @ " << Shared::getCurrentPath() << " > ";
			std::cout << outputStream.str();

			// Get command input from the user
			std::getline(std::cin, commandBuffer);
			std::cin.clear();

			// Print the command output to the user
			std::cout << parseCommand(client, commandBuffer);
		}
	}
}
