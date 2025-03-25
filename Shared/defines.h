#pragma once

#define PORT 8186

// Utility macros
/* @brief Cast a pointer from one type to another type, by first casting to a void pointer
*  @param[in] type	The type to cast to. The pointer '*' is implied, and does not need to be specified.
*  @param[in] ptr	The input pointer to cast
*  @return			A pointer of type 'type*' pointing to the same address as 'ptr'
*/
#define ptr_cast(type, ptr)		static_cast<type*>(static_cast<void*>(ptr))

/* @brief Cast a constant pointer from one type to another constant type, by first casting to a constant void pointer
*  @param[in] type	The type to cast to. const is implied, and does not need to be specified
*  @param[in] ptr	The input pointer to cast
*  @return			A pointer of type 'const type*' pointing to the same address as 'ptr'
*/
#define cptr_cast(type, ptr)	static_cast<const type*>(static_cast<const void*>(ptr))

// Packet
#define MAX_DATA_SIZE	(0xFFFF)
#define HEADER_SIZE		(6)
#define FOOTER_SIZE		(1)
#define MAX_PACKET_SIZE (HEADER_SIZE + MAX_DATA_SIZE + FOOTER_SIZE)
#define MIN_PACKET_SIZE (HEADER_SIZE + FOOTER_SIZE)

// Application Directories
#define ROOT_DIR	"GoogCraftImages/"
#define LOGS_DIR	"logs/"
#define FILES_DIR	"files/"

// Client UI
#define CUI_MAX_COMMAND_LENGTH	256
#define CUI_CMD_HELP		"help"
#define CUI_CMD_UPLOAD		"push" // REQ-CLT-030
#define CUI_CMD_DOWNLOAD	"pull"
#define CUI_CMD_INFO		"info" // REQ-CLT-050
#define CUI_CMD_LS			"ls"
#define CUI_CMD_MSG			"msg" // REQ-CLT-020
#define CUI_CMD_TELEM		"telem" // REQ-CLT-040

// Terminal Colours
#define TERM_ESC	"\033["
#define TERM_RESET		TERM_ESC "0m"
#define TERM_FG_BLACK	TERM_ESC "30m"
#define TERM_FG_RED		TERM_ESC "31m"
#define TERM_FG_GREEN	TERM_ESC "32m"
#define TERM_FG_YELLOW	TERM_ESC "33m"
#define TERM_FG_BLUE	TERM_ESC "34m"
#define TERM_FG_PURPLE	TERM_ESC "35m"
#define TERM_FG_CYAN	TERM_ESC "36m"
#define TERM_FG_WHITE	TERM_ESC "37m"

#define TERM_BG_BLACK	TERM_ESC "40m"
#define TERM_BG_RED		TERM_ESC "41m"
#define TERM_BG_GREEN	TERM_ESC "42m"
#define TERM_BG_YELLOW	TERM_ESC "43m"
#define TERM_BG_BLUE	TERM_ESC "44m"
#define TERM_BG_PURPLE	TERM_ESC "45m"
#define TERM_BG_CYAN	TERM_ESC "46m"
#define TERM_BG_WHITE	TERM_ESC "47m"
