#include "Commands.h"
#include "defines.h"

namespace UI {
	const std::string parseCommand(Shared::Socket& socket, const std::string& cmdLine) {
		if (cmdLine == CUI_CMD_HELP) {
			return cmdHelp();
		}
		else if (cmdLine == CUI_CMD_UPLOAD) {
			return cmdPush(socket, cmdLine);
		}
		else if (cmdLine == CUI_CMD_DOWNLOAD) {
			return cmdPull(socket, cmdLine);
		}
		else if (cmdLine == CUI_CMD_INFO) {
			return cmdInfo(socket);
		}
		else if (cmdLine == CUI_CMD_LS) {

		}
		else if (cmdLine == CUI_CMD_MSG) {

		}
		else if (cmdLine == CUI_CMD_TELEM) {

		}

		return "Command does not exist or was typed incorrectly. Use 'help' to find existing commands.\n";
	}

	const std::string cmdHelp() {
		return
			TERM_BG_BLACK TERM_FG_WHITE " ---= GoogCraftImages - Client CLI Help Menu =---\n" TERM_RESET\
			" help\t\t\tDisplay this help menu\n"\
			" push <filename>\tUpload a file to the server\n"\
			" pull <filename>\tDownload a file from the server\n"\
			" info\t\t\tQuery the server for general information\n";
	}

	const std::string cmdPush(Shared::Socket& socket, const std::string& cmdLine) {
		return "";
	}

	const std::string cmdPull(Shared::Socket& socket, const std::string& cmdLine) {
		return "";
	}

	const std::string cmdInfo(Shared::Socket& socket) {
		return "";
	}

	const std::string cmdLs() {
		return "";
	}

	const std::string cmdMsg(Shared::Socket& socket, const std::string& cmdLine) {
		return "";
	}

	const std::string cmdTelem(Shared::Socket& socket, const std::string& cmdLine) {
		return "";
	}
}