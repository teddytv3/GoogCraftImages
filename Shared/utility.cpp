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

	/* @brief Display the files in some directory
	*  @param[in] path	The static path to the files
	*/
	void displayFiles(std::string const& path) {
		// Only if the file path exists...
		if (std::filesystem::exists(path)) {
			std::cout << "Files in '" << path << "':" << std::endl;
			// Loop through all files
			for (const auto& entry : std::filesystem::directory_iterator(path)) {
				std::cout << entry.path().filename() << std::endl;
			}
		}
		else {
			std::cout << "'" << path << "' does not exist." << std::endl;
		}
	}

	void DisplayReceivedFiles() {

		std::string path = Shared::getRootPath();
		path += FILES_DIR;

		displayFiles(path);
	}

	void DisplayLogFiles() {
		std::string path = Shared::getRootPath();
		path += LOGS_DIR;

		displayFiles(path);
	}

	std::string findFilePath(const std::string& filename) {
		std::string path = getRootPath();
		
		// Check logs dir
		if (std::filesystem::exists(path + LOGS_DIR + filename)) {
			path += LOGS_DIR + filename;
		}

		// Default - The path that we will save the file to
		else {
			path += FILES_DIR + filename;
		}

		return path;
	}
}