#pragma once
#include <string>

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

}
