#pragma once

#define PORT 8186

// Utility macros
#define ptr_cast(type, ptr) static_cast<type*>(static_cast<void*>(ptr))

// Packet
#define MAX_DATA_SIZE	(0xFFFF)
#define HEADER_SIZE		(6)
#define FOOTER_SIZE		(1)
#define MAX_PACKET_SIZE (HEADER_SIZE + MAX_DATA_SIZE + FOOTER_SIZE)
#define MIN_PACKET_SIZE (HEADER_SIZE + FOOTER_SIZE)