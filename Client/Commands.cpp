#include "Commands.h"
#include "defines.h"
#include "logger.h"
#include "utility.h"
#include "stream.h"

#include <fstream>

namespace UI {
	const std::string parseCommand(Shared::Socket& socket, std::vector<std::string>& args) {
		if (args.size() <= 0) {
			return "";
		}

		std::string result;
		std::string command = args[0];
		// Remove the first element
		args.erase(args.begin(), args.begin() + 1);

		if (command == CUI_CMD_HELP) {
			result = cmdHelp();
		}
		else if (command == CUI_CMD_UPLOAD) {
			result = cmdPush(socket, args);
		}
		else if (command == CUI_CMD_DOWNLOAD) {
			result = cmdPull(socket, args);
		}
		else if (command == CUI_CMD_INFO) {
			result = cmdInfo(socket);
		}
		else if (command == CUI_CMD_LS) {
			result = cmdLs();
		}
		else if (command == CUI_CMD_MSG) {
			result = cmdMsg(socket, args);
		}
		else if (command == CUI_CMD_TELEM) {
			result = cmdTelem(socket, args);
		}
		else {
			result = "Command does not exist or was typed incorrectly. Use 'help' to find existing commands.\n";
		}

		// Now return the result as a single point of exit
		return result;
	}

	const std::string cmdHelp() {
		return
			TERM_BG_BLACK TERM_FG_WHITE " ---= GoogCraftImages - Client CLI Help Menu =---\n" TERM_RESET\
			" help\t\t\tDisplay this help menu\n"\
			" push <filename>\tUpload a file to the server\n"\
			" pull <filename>\tDownload a file from the server\n"\
			" info\t\t\tQuery the server for general information\n"\
			" ls\t\t\tList files in program directories.\n"\
			" msg <message>\t\tSend a dynamically-sized message to the server.\n"\
			" telem <on/off>\t\tEnable or disable telemetry.\n";
	}

	const std::string cmdPush(Shared::Socket& socket, const std::vector<std::string>& args) {
		std::string result = "";

		int streamReturn = Shared::streamOutFileOverSocket(socket, args[0]);

		if (streamReturn == 0) {
			result = "Successfully sent file " + args[0] + " to the server!\n";
		}
		else {
			result = "Error " + std::to_string(streamReturn) + " when sending sequences.\n";
		}
		
		return result;
	}

	const std::string cmdPull(Shared::Socket& socket, const std::vector<std::string>& args) {
		return "";
	}

	const std::string cmdInfo(Shared::Socket& socket) {
		return "";
	}

	const std::string cmdLs() {
		Shared::DisplayLogFiles();
		Shared::DisplaySentFiles();
		return "";
	}

	const std::string cmdMsg(Shared::Socket& socket, const std::vector<std::string>& args) {
		return "";
	}

	const std::string cmdTelem(Shared::Socket& socket, const std::vector<std::string>& args) {
		return "";
	}
}