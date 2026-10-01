#pragma once

#include "packet.hpp"

#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>

class JitterBuffer {
private:
    std::map<std::uint32_t, Packet> buffer;
    std::uint32_t expected_sequence;

public:
    JitterBuffer(std::uint32_t starting_sequence);

    void insert(Packet packet);
    std::optional<Packet> popNext();

    std::size_t size() const;
    std::uint32_t expectedSequence() const;
    void skipExpected();
};