#pragma once
#include <string>
#include "Socket.h"

#define DisplaySentFiles DisplayReceivedFiles

namespace Shared {
	/* @brief Get the path to the program location within %AppData%
	*  @return	A string containing a path to the root directory of this project
	*/
	extern std::string getRootPath();

	/* @brief Get the current working directory
	*  @return	A string representing the full path of the current working directory
	*/
	extern std::string getCurrentPath();

	extern void DisplayReceivedFiles();

	extern void DisplayLogFiles();

	/* @brief Get a full static file path based on a filename. Searches within the ROOT project directory
	*  @param[in] filename	The filename of the file to get the full path for. A full path is generated even if the file does not exist. 
	*  @return				Either the full static path of where the file was found, or the full static path of where the file should be placed.
	*/
	extern std::string findFilePath(const std::string& filename);

}
