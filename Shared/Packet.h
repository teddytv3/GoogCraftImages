#include <iostream>
#include <vector>
#include <variant>
#include <cstdint>
#include <cstring>

enum ActionType : uint8_t {
    ACT_UPLOAD,
    ACT_DOWNLOAD,
    ACT_DELETE,
    ACT_POSITION
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

struct PacketHeader {
    ActionType actionID;
    PktType pktType;
    uint16_t sequenceNum;
    uint16_t dataSize;
};

union PacketData {
    TelemetryData telemetry;
    //std::vector<uint8_t> images;
    //std::string googleEarthPhoto;
};


class Packet {
private:
    PacketHeader header;

    //PacketData data;
    uint8_t* data;
    uint8_t pktChecksum;

public:
    /* @brief Create an empty packet with everything nullified
    */
    Packet();

    Packet(ActionType id, PktType type, uint16_t seqNum, uint16_t dataSize, uint8_t* pktData = nullptr);

    /* @brief Destructor. Free the data buffer if it's allocated
    */
    ~Packet();
    
    /* @brief Prepare a packet based on data within a buffer
    *  @param[in] newHeader The packet header as a data structure
    *  @param[in] buffer    A pointer to the beginning of the data buffer
    *  @param[in] size      The size of the buffer. The number of bytes to use.
    *  @return              True upon error, false otherwise
    */
    bool setPacket(PacketHeader const& newHeader, char* buffer, unsigned int size);

    /* @brief Copy a pointer to some data into this packet. Sets the data size, copies data, then calculates checksum.
    *  @param[in] buffer    The source data buffer to copy.
    *  @param[in] size      The number of bytes within buffer to copy.
    *  @return              True upon error, false otherwise
    */
    bool copyData(uint8_t* buffer, uint16_t size);

    /* @brief Calculate the checksum on the packet header and dynamically sized data field
    *  @return  The final calculated checksum of the packet
    */
    uint8_t calculateChecksum() const;


    void displayInfo() const;
};
