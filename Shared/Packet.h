#include <iostream>
#include <vector>
#include <variant>
#include <cstdint>
#include <cstring>

enum class ActionType {
    UPLOAD,
    DOWNLOAD,
    DELETE,
    POSITION
};

enum class PktType : uint8_t {
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

union PacketData {
    TelemetryData telemetry;
    //std::vector<uint8_t> images;
    //std::string googleEarthPhoto;
};


class Packet {
private:
    uint8_t actionID;
    PktType pktType;
    uint16_t sequenceNum;
    uint16_t dataSize;
    PacketData data;
    uint8_t pktChecksum;

public:
    Packet(uint8_t id, PktType type, uint16_t seqNum, const PacketData& pktData)
        : actionID(id), pktType(type), sequenceNum(seqNum), data(pktData);

    uint32_t calculateChecksum() const;


    void displayInfo() const;
};
