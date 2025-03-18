#include "logger.h"

/*
 * @brief This function logs data to a file, specified by either the client or the server for proper debugging. 
 * @param filename This parameter is a string variable that contains the name of the file to write to. This is separated from the log folder to ensure client and server have separate files.
 * @param successFlag This parameter is a flag that indicates if a packet was sent/received successfully or not.
 */
void log(std::string filename, uint8_t successFlag) {
	std::string logFolder = "../logs/";	//Using a different file name to separate between Client and Server logs. They should still live in the same folder however.
	std::ofstream file;
	file.open(logFolder.append(filename), std::ios_base::app);
	file << std::chrono::system_clock::now() << ":	" << successFlag << std::endl;
	file.close();
}