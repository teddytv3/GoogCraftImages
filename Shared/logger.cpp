#include "logger.h"
#include "defines.h"
#include <sstream>
#include <iostream>
#include "utility.h"

#include <filesystem>

namespace Shared {
	/*
	 * @brief This function logs data to a file, specified by either the client or the server for proper debugging.
	 * @param filename This parameter is a string variable that contains the name of the file to write to. This is separated from the log folder to ensure client and server have separate files.
	 * @param successFlag This parameter is a flag that indicates if a packet was sent/received successfully or not.
	 */
	void log(std::string filename, int8_t successFlag, std::string msg) {
		std::ostringstream stream;
		stream << std::chrono::system_clock::now().time_since_epoch().count() << ":	" << static_cast<int32_t>(successFlag) << " '" << msg << "'";

		// Get the %AppData% path
		std::string path = getRootPath() + LOGS_DIR;
		// Create the path if it does not yet exist
		std::filesystem::create_directories(path);

		std::string filePath = path + filename;	//Using a different file name to separate between Client and Server logs. They should still live in the same folder however.
		std::ofstream file;
		file.open(filePath, std::ios_base::app);
		if (file.fail() || file.bad()) {
			std::cerr << "Failed to open log file at '" << filePath << "'" << std::endl;
		}

		file << stream.str() << std::endl;
		file.close();

		// Echo to stdout
		std::cout << filename << "-" << stream.str() << std::endl;
	}
}