#include "server_interface.h"
#include "utility.h"
#include <defines.h>
#include <iostream>
#include <string>
#include "utility.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace Server {
	void DisplayOptions() {
		while (true) {
			std::cout << "***************************" << std::endl;
			std::cout << "*                         *" << std::endl;
			std::cout << "* 1. View Log Files       *" << std::endl;
			std::cout << "* 2. View Received Files  *" << std::endl;
			std::cout << "*                         *" << std::endl;
			std::cout << "***************************" << std::endl;

			char input;

			std::cin >> input;

			switch (input) {
				case '1':
					Shared::DisplayLogFiles();
					break;
				case '2':
					Shared::DisplayReceivedFiles();
					break;
				default: {
					std::cout << "Bad Input " << std::endl;
					break;
				}
				
			}
		}
	}
}
