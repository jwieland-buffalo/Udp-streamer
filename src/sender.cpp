#include "packet.hpp"
#include "network_simulator.hpp"
#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

int main() {
    boost::asio::io_context io;

    auto receiver = boost::asio::ip::udp::endpoint(
        boost::asio::ip::make_address("127.0.0.1"),
        9000
    );

    NetworkSimulator simulator(io, receiver);

    for (std::uint32_t sequence = 0; sequence < 100; ++sequence) {
        std::uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
        ).count();
        
        Packet packet(sequence, timestamp, std::array<char, 160>{});
        simulator.send(packet);
    }
    io.run();

    std::cout << "Finished sending packets.\n";
}