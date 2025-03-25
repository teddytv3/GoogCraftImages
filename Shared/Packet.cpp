#include "Packet.h"
#include <iostream>
#include "defines.h"
#include "logger.h"

namespace Shared {
    Packet::Packet() {
        // Nullify all fields
        if (::memset(this, 0, sizeof(Packet)) != this) {
            log("packet.log", -1, "Unexpected error when nullifying packet in Packet() constructor");
        }
    }

    Packet::Packet(ActionType id, PktType type, uint16_t seqNum, uint16_t dataSize, const char* pktData) {
        this->header.actionID = id;
        this->header.pktType = type;
        this->header.sequenceNum = seqNum;
        this->header.dataSize = dataSize;

        // Copy the provided packet data into this packet
        if (this->copyData(pktData, dataSize) == true) {
            log("packet.log", -1, "Failed to copy data into new packet in packet constructor. No data provided when a checksum was expected. Calculating new checksum.");
            // Recalculate checksum - Assume none was provided. Just create a packet using the header data.
            this->pktChecksum = this->calculateChecksum();
        }
    }

    Packet::~Packet() {
        if (this->data != nullptr) {
            delete[] this->data;

            // Reset to nullptr just in case
            this->data = nullptr;
        }
    }

    /* @brief Get a constant reference to the packet header object
    *  @return  A constant reference to the packet header object
    */
    PacketHeader const& Packet::getPacketHeader() const {
        return this->header;
    }

    /* @brief Get a constant pointer to the data buffer
    *  @return  A constant pointer to the data buffer
    */
    const char* Packet::getData() const {
        return this->data;
    }

    /* @brief Get a copy of the packet's checksum
    *  @return  A copy of the packet's checksum
    */
    const uint8_t Packet::getChecksum() const {
        return this->pktChecksum;
    }



    bool Packet::setPacket(PacketHeader const& newHeader, char* buffer, unsigned int size) {
        // Check for input errors
        // Since buffer is the data + checksum, the minimum value for 'size' is FOOTER_SIZE
        if (buffer == nullptr || size < FOOTER_SIZE) {
            log("packet.log", -1, "setPacket received null buffer or low packet size");
            return true;
        }

        // Attempt to set the packet
        // At this point we know the packet has enough data to contain a header
        this->header = newHeader;

        // CHeck to make sure there's enough bytes in the buffer for 'dataSize'
        const uint32_t DATA_BYTES_FOUND = size - FOOTER_SIZE;

        // If not enough bytes were received... return true for error
        if (DATA_BYTES_FOUND < this->header.dataSize) {
            log("packet.log", -2, "Could not set packet using buffer. Not enough data bytes provided.");
            std::cerr << "Could not set packet using buffer. Not enough data bytes provided. Expected " << this->header.dataSize << " bytes but found " << DATA_BYTES_FOUND << std::endl;
            return true;
        }

        // Copy the packet data and set checksum
        if (this->copyData(buffer, this->header.dataSize)) {
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

    void Packet::makeTelemetry(PktType response) {
        // Free the data within the packet
        if (this->data != nullptr) {
            delete[] this->data;
            this->data = nullptr;
        }

        // Set the header variables
        this->header.dataSize = 0;
        this->header.pktType = response;
        this->header.sequenceNum = 0;

        // Update the checksum
        this->pktChecksum = this->calculateChecksum();
    }

    bool Packet::copyData(const char* buffer, unsigned short int size) {
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
            this->data = new char[size];
            if (this->data == nullptr) {
                std::cerr << "Failed to allocate data buffer of size " << size << ". Message from Server: \"I'm hungry for dedidaded wam :(\"" << std::endl;
                return true;
            }

            // Copy data
            ::memcpy(this->data, buffer, size);

            // Set the size within the header
            this->header.dataSize = size;
        }

        // Calculate the checksum
        this->pktChecksum = this->calculateChecksum();

        // No err
        return false;
    }

    uint8_t Packet::calculateChecksum() const {
        // Calculate the checksum on the header to begin
        uint8_t checksum = this->header.actionID ^ static_cast<uint8_t>(this->header.pktType) ^ this->header.sequenceNum ^ this->header.dataSize;

        if (this->data != nullptr) {
            // Calculate the checksum on the data 
            for (uint16_t i = 0; i < this->header.dataSize; i++) {
                checksum ^= this->data[i];
            }
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

    unsigned int Packet::getPacketSize() const {
        return MIN_PACKET_SIZE + this->header.dataSize;
    }

    bool Packet::serialize(char* buffer, const unsigned int size) const {
        if (nullptr == buffer) {
            log("packet.log", 1, "Failed to serialize packet");
            return true;
        }

        // First, copy the header and increase the iterator
        ::memcpy(buffer, &this->header, sizeof(this->header));
        buffer += sizeof(this->header);

        // Now copy the data field
        ::memcpy(buffer, this->data, this->header.dataSize);
        buffer += this->header.dataSize;

        // Finally, append the checksum
        ::memcpy(buffer, &this->pktChecksum, sizeof(this->pktChecksum));

        return false;
    }
}