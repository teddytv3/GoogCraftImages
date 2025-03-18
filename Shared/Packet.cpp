#include "Packet.h"
Packet::Packet(uint8_t id, PktType type, uint16_t seqNum, const PacketData& pktData)
    : actionID(id), pktType(type), sequenceNum(seqNum), data(pktData) {
    dataSize = sizeof(pktData);
    pktChecksum = calculateChecksum();
}

Packet::uint32_t calculateChecksum() const {
    return actionID ^ static_cast<uint8_t>(pktType) ^ sequenceNum ^ dataSize;
}


Packet::void displayInfo() const {
    std::cout << "Action ID: " << actionID << "\n"
        << "Packet Type: " << static_cast<int>(pktType) << "\n"
        << "Sequence Number: " << sequenceNum << "\n"
        << "Data Size: " << dataSize << "\n"
        << "Checksum: " << pktChecksum << "\n";
}