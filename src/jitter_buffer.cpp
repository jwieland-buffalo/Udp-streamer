#include "jitter_buffer.hpp"

JitterBuffer::JitterBuffer(std::uint32_t starting_sequence)
    : expected_sequence(starting_sequence)
{
}

void JitterBuffer::insert(Packet packet) {
    // Ignore packets that are already too late for playback.
    if (packet.getSequenceNumber() < expected_sequence) {
        return;
    }

    buffer.insert_or_assign(packet.getSequenceNumber(), packet);
}

std::optional<Packet> JitterBuffer::popNext() {
    auto it = buffer.find(expected_sequence);

    if (it == buffer.end()) {
        return std::nullopt;
    }

    Packet packet = it->second;
    buffer.erase(it);
    ++expected_sequence;

    return packet;
}

std::size_t JitterBuffer::size() const {
    return buffer.size();
}

std::uint32_t JitterBuffer::expectedSequence() const {
    return expected_sequence;
}

void JitterBuffer::skipExpected() {
    ++expected_sequence;
}