#include "../src/packet.hpp"
#include <cassert>
#include <iostream>
int main() {
    // Known values for testing
    std::uint32_t seq_num = 42;
    std::uint64_t ts = 1234567890123456789ULL;
    std::array<char, 160> data;
    for (std::size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<char>(i);
    }
    Packet packet(seq_num, ts, data);
    auto serialized = serialize(packet);
    assert(serialized.size() == 172);
    Packet deserialized = deserialize(serialized);
    assert(deserialized.getSequenceNumber() == seq_num);
    assert(deserialized.getTimestamp() == ts);
    assert(deserialized.getPayload() == data);
    std::cout << "Packet test passed.\n";
    return 0;
}