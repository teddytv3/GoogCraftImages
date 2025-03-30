#pragma once
#include <string>
#include "Socket.h"

namespace UI {
	/* @brief Parse a received command from the user, call the appropriate command function. Return the appropriate result. 
	 * @param[in] args		The arguments provided in the command line
	 * @return				The output from the command that was executed, to be displayed to hte user
	*/
	const std::string parseCommand(Shared::Socket& socket, std::vector<std::string>& args);

	/* @brief Perform a help command and return the response
	*  @return	Get the help menu response to be displayed to the user
	*/
	const std::string cmdHelp();

	/* @brief Upload a file to the server via command. 
	*  @param[in] cmdLine	The input command line. Contains the expected filename to upload. MUST NOT CONTAIN SPACES
	*  @return				The output from the push command. Includes success or failure messages.
	*/
	const std::string cmdPush(Shared::Socket& socket, const std::vector<std::string>& args);

	/* @brief Download a file from the server via commad.
	*  @param[in] cmdLine	The input command line. Contains the expected filename to download. MUST NOT CONTAIN SPACES
	*  @return				The output from the download sequence. Includes success or failure messages, and a path to the downloaded file. 
	*/
	const std::string cmdPull(Shared::Socket& socket, const std::vector<std::string>& args);

	/* @brief Query the server for information such as arrival time, response time, file count, etc.
	*  @return	Information about the server.
	*/
	const std::string cmdInfo(Shared::Socket& socket);

	const std::string cmdLs();

	const std::string cmdMsg(Shared::Socket& socket, const std::vector<std::string>& args);

	const std::string cmdTelem(Shared::Socket& socket, const std::vector<std::string>& args);
}