#pragma once
#include <iostream>
#include <vector>
#include <variant>
#include <cstdint>
#include <cstring>

namespace Shared {
    enum ActionType : uint8_t {
        ACT_UPLOAD,
        ACT_DOWNLOAD,
        ACT_DELETE,
        ACT_POSITION,
        ACT_MESSAGE,
        ACT_TELEMETRY
    };

    enum PktType : uint8_t {
        ACTION = 0,
        ACK = 1,
        NACK = 2
    };

    struct TelemetryData {
        double longitude;
        double latitude;
        double height;
        double fuel;
    };

    #pragma pack(push, 1)
    struct PacketHeader {
        ActionType actionID;
        PktType pktType;
        uint16_t sequenceNum;
        uint16_t dataSize;
    };
    #pragma pack(pop)

    class Packet {
    private:
        PacketHeader header;

        //PacketData data;
        char* data;
        uint8_t pktChecksum;

    public:
        /* @brief Create an empty packet with everything nullified
        */
        Packet();

        Packet(ActionType id, PktType type, uint16_t seqNum, uint16_t dataSize, const char* pktData = nullptr);

        /* @brief Destructor. Free the data buffer if it's allocated
        */
        ~Packet();

        // Getters
        /* @brief Get a constant reference to the packet header object
        *  @return  A constant reference to the packet header object
        */
        PacketHeader const& getPacketHeader() const;

        /* @brief Get a constant pointer to the data buffer
        *  @return  A constant pointer to the data buffer
        */
        const char* getData() const;

        /* @brief Get a copy of the packet's checksum
        *  @return  A copy of the packet's checksum
        */
        const uint8_t getChecksum() const;

        /* @brief Prepare a packet based on data within a buffer
        *  @param[in] newHeader The packet header as a data structure
        *  @param[in] buffer    A pointer to the beginning of the data buffer
        *  @param[in] size      The size of the buffer. The number of bytes to use.
        *  @return              True upon error, false otherwise
        */
        bool setPacket(PacketHeader const& newHeader, const char* buffer, unsigned int size);

        /* @brief Turn a command packet into a response packet
        *  @param[in] The packet type to send in response
        */
        void makeTelemetry(PktType response);

        /* @brief Copy a pointer to some data into this packet. Sets the data size, copies data, then calculates checksum.
        *  @param[in] buffer    The source data buffer to copy.
        *  @param[in] size      The number of bytes within buffer to copy.
        *  @return              True upon error, false otherwise
        */
        bool copyData(const char* buffer, unsigned short int size);

        /* @brief Calculate the checksum on the packet header and dynamically sized data field
        *  @return  The final calculated checksum of the packet
        */
        uint8_t calculateChecksum() const;


        void displayInfo() const;

        /* @brief Get the size of the full packet
        *  @return	The size of the packet in bytes. Returns at least MIN_PACKET_SIZE
        */
        unsigned int getPacketSize() const;

        /* @brief Serialize the packet into a buffer
        *  @param[out] buffer   A pointer to the buffer that will be populated with the packet
        *  @param[in] size      A value representing the number of bytes allocated and available within the buffer.
        *  @return  True upon failure. False otherwise
        */
        bool serialize(char* buffer, const unsigned int size) const;

    };
}