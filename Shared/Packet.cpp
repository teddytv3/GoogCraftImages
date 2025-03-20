#include "Packet.h"
#include <iostream>
#include "defines.h"

Packet::Packet() {
    // Nullify all fields
    ::memset(this, 0, sizeof(Packet));
}

Packet::Packet(ActionType id, PktType type, uint16_t seqNum, uint16_t dataSize, uint8_t* pktData) {
    this->header.actionID = id;
    this->header.pktType = type;
    this->header.sequenceNum = seqNum;
    this->header.dataSize = dataSize;

    // Copy the provided packet data into this packet
    this->copyData(pktData, dataSize);
}

Packet::~Packet() {
    if (this->data != nullptr) {
        delete[] this->data;

        // Reset to nullptr just in case
        this->data = nullptr;
    }
}

bool Packet::setPacket(PacketHeader const& newHeader, char* buffer, unsigned int size) {
    // Check for input errors
    if (buffer == nullptr || size < MIN_PACKET_SIZE) {
        std::cerr << "Could not set packet using NULL buffer or low packet size" << std::endl;
        return true;
    }

    // Attempt to set the packet
    // At this point we know the packet has enough data to contain a header
    this->header = newHeader;

    // CHeck to make sure there's enough bytes in the buffer for 'dataSize'
    const uint32_t DATA_BYTES_FOUND = size - HEADER_SIZE - FOOTER_SIZE;

    // If not enough bytes were received... return true for error
    if (DATA_BYTES_FOUND < this->header.dataSize) {
        std::cerr << "Could not set packet using buffer. Not enough data bytes provided. Expected " << this->header.dataSize << " bytes but found " << DATA_BYTES_FOUND << std::endl;
        return true;
    }

    // Copy the packet data and set checksum
    if (this->copyData(ptr_cast(uint8_t, buffer), this->header.dataSize)) {
        return true;
    }

    // Finally, set the checksum if it was received in the buffer
    // If the size of the buffer was one larger than the header's data size, read the final byte as the new checksum
    if (size == this->header.dataSize + 1) {
        this->pktChecksum = buffer[size - 1];
    }

    // Done - success if made it here
    return false;
}

bool Packet::copyData(uint8_t* buffer, uint16_t size) {
    // Ensure that the buffer exists
    if (buffer == nullptr) {
        std::cerr << "Could not copy packet data. Buffer was null." << std::endl;
        return true;
    }
    else {
        
        // If data is already allocated, free it.
        if (this->data != nullptr) {
            delete[] this->data;
            this->data = nullptr;
        }

        // Allocate new data
        this->data = new uint8_t[size];
        if (this->data == nullptr) {
            std::cerr << "Failed to allocate data buffer of size " << size << ". Message from Server: \"I'm hungry for dedidaded wam :(\"" << std::endl;
            return true;
        }

        // Copy data
        ::memcpy(this->data, buffer, size);

        // Set the size within the header
        this->header.dataSize = size;

        // Calculate the checksum
        this->pktChecksum = this->calculateChecksum();
    }

    // No err
    return false;
}

uint8_t Packet::calculateChecksum() const {
    // Calculate the checksum on the header to begin
    uint8_t checksum = this->header.actionID ^ static_cast<uint8_t>(this->header.pktType) ^ this->header.sequenceNum ^ this->header.dataSize;

    // Calculate the checksum on the data 
    for (uint16_t i = 0; i < this->header.dataSize; i++) {
        checksum ^= this->data[i];
    }

    return checksum;
}


void Packet::displayInfo() const {
    std::cout << "Action ID: " << this->header.actionID << "\n"
        << "Packet Type: " << static_cast<int>(this->header.pktType) << "\n"
        << "Sequence Number: " << this->header.sequenceNum << "\n"
        << "Data Size: " << this->header.dataSize << "\n"
        << "Checksum: " << this->pktChecksum << "\n";
}