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
