#include "../src/jitter_buffer.hpp"
#include <cassert>
#include <array>

Packet makePacket(std::uint32_t sequence) {
    std::array<char, 160> payload{};
    return Packet(sequence, 0, payload);
}

int main() {
    JitterBuffer buffer(90);

    buffer.insert(makePacket(91));
    buffer.insert(makePacket(93));
    buffer.insert(makePacket(90));
    buffer.insert(makePacket(92));

    auto packet = buffer.popNext();
    assert(packet.has_value());
    assert(packet->getSequenceNumber() == 90);

    packet = buffer.popNext();
    assert(packet.has_value());
    assert(packet->getSequenceNumber() == 91);

    packet = buffer.popNext();
    assert(packet.has_value());
    assert(packet->getSequenceNumber() == 92);

    packet = buffer.popNext();
    assert(packet.has_value());
    assert(packet->getSequenceNumber() == 93);
}