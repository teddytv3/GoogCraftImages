#include "server_interface.h"
#include "utility.h"
#include <defines.h>
#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;
using namespace Shared;

namespace Server {
	void DisplayReceivedFiles() {

		std::string path = getRootPath();
		path += FILES_DIR;

		for (const auto& entry : fs::directory_iterator(path))
			std::cout << entry.path() << std::endl;
	}

	void DisplayLogFiles() {
		std::string path = getRootPath();
		path += LOGS_DIR;

		for (const auto& entry : fs::directory_iterator(path))
			std::cout << entry.path() << std::endl;
	}

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
					DisplayLogFiles();
					break;
				case '2':
					DisplayReceivedFiles();
					break;
				default: {
					std::cout << "Bad Input " << std::endl;

					
					break;
				}
				
			}
		}
	}
}
