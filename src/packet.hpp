#pragma once
#include <array>
#include <cstdint>
#include <vector>

class Packet {
private:
    std::uint32_t sequence_number;
    std::uint64_t timestamp;
    std::array<char, 160> payload;
public:
    Packet(std::uint32_t seq_num, std::uint64_t ts, const std::array<char, 160>& data)
        : sequence_number(seq_num), timestamp(ts), payload(data) {}
        
    std::uint32_t getSequenceNumber() const { return sequence_number; }
    std::uint64_t getTimestamp() const { return timestamp; }
    const std::array<char, 160>& getPayload() const { return payload; }
};

std::vector<std::uint8_t> serialize(const Packet& packet);
Packet deserialize(const std::vector<std::uint8_t>& bytes);