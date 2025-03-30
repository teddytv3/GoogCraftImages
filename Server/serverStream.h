#pragma once
#include "states.h"
#include "Packet.h"

namespace Server {
	struct SequenceContainer {
		char* dataPtr;
		uint16_t bytes;
	};


	/* @brief Handle the initial packet of an upload stream on the server-side. 
	*  @param[in/out] state				A reference to the state object that will be overwritten and read
	*  @param[in] gotPacket				The received packet object
	*  @param[in/out] uploadFilename	The filename object to store the received filename from this packet
	*  @return							True upon error. False otherwise.
	*/
	extern bool handleUploadPacketOne(Server::State& state, Shared::Packet const& gotPacket, std::string& uploadFilename);

	/* @brief Handle the data packets of an upload stream on the server-side
	*  @param[in] gotPacket				A reference to the packet object
	*  @param[in/out] sequences			A references to the sequences vector that contains the list of dynamically allocated sequences.
	*  @param[out] pktResponse			A reference to the pktResponse value holding the return value 
	*  @return							True upon error. False otherwise.
	*/
	extern bool handleUploadPacketData(Shared::Packet const& gotPacket, std::vector<SequenceContainer>& sequences);

	/*
	*  @param[in] gotPacket				The received packet object
	*  @param[in/out] sequences			A references to the sequences vector that contains the list of dynamically allocated sequences.
	*  @param[in/out] state				A reference to the state object that will be overwritten and read
	*  @param[in/out] uploadFilename	The filename object to store the received filename from this packet
	*  @return							True if a sequence was missing, false otherwise. 
	*/
	extern bool handleUploadPacketFinal(Shared::Packet const& gotPacket, std::vector<SequenceContainer>& sequences, Server::State& state, std::string& uploadFilename);
}