#include "packet.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>

std::vector<std::uint8_t> serialize(const Packet& packet){
    // Serialize the packet into a vector of bytes for transmission
    std::vector<std::uint8_t> bytes;
    // Serialize sequence number (4 bytes)
    for (int i = 3; i >= 0; --i) {
    bytes.push_back(
        (packet.getSequenceNumber() >> (i * 8)) & 0xFF
    );
    }
    // Serialize timestamp (8 bytes)
    for (int i = 7; i >= 0; --i) {
    bytes.push_back(
        (packet.getTimestamp() >> (i * 8)) & 0xFF
    );
    }
    // Serialize payload (160 bytes)
    const auto& payload = packet.getPayload();
    bytes.insert(bytes.end(), payload.begin(), payload.end());
    return bytes;
}

Packet deserialize(const std::vector<std::uint8_t>& bytes) {
    if(bytes.size() < 172) { // 4 bytes for sequence number, 8 bytes for timestamp, 160 bytes for payload
        throw std::runtime_error("not enough bytes to deserialize packet");
    }
    std::uint32_t sequence_number = 0;
    for (int i = 0; i < 4; ++i) {
        sequence_number |= static_cast<std::uint32_t>(bytes[i]) << ((3 - i) * 8);
    }
    std::uint64_t timestamp = 0;
    for (int i = 0; i < 8; ++i) {
        timestamp |= static_cast<std::uint64_t>(bytes[4 + i]) << ((7 - i) * 8);
    }
    std::array<char, 160> payload;
    std::copy(bytes.begin() + 12, bytes.begin() + 172, payload.begin());
    return Packet(sequence_number, timestamp, payload);
}