#include "utility.h"
#include "defines.h"

#include <windows.h>
#include <filesystem>
#include <ShlObj.h>
#include <iostream>

namespace Shared {
	std::string getRootPath() {
		std::string finalPath;

		// Get the %AppData% path
		std::filesystem::path path;
		PWSTR tempWindowsPath;
		auto pathRet = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &tempWindowsPath);

		if (pathRet != S_OK) {
			CoTaskMemFree(tempWindowsPath);
			std::cerr << "Failed to find %AppData% path." << std::endl;
			exit(1);
		}

		// Copy the path to a sane format
		path = tempWindowsPath;

		// Free the windows memory
		CoTaskMemFree(tempWindowsPath);

		// Return the final path string
		return path.generic_string() + "/" + ROOT_DIR;
	}

	std::string getCurrentPath() {
		return std::filesystem::current_path().generic_string();
	}

	void DisplayReceivedFiles() {

		std::string path = Shared::getRootPath();
		path += FILES_DIR;

		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			std::cout << entry.path() << std::endl;
		}
	}

	void DisplayLogFiles() {
		std::string path = Shared::getRootPath();
		path += LOGS_DIR;

		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			std::cout << entry.path() << std::endl;
		}
	}
}