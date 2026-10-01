#include "network_simulator.hpp"
#include <random>
#include <iostream>
std::random_device rd;
std::mt19937 generator(rd());
std::uniform_int_distribution<int> distribution(1, 20);
std::uniform_int_distribution<int> delayDistribution(0, 100);

NetworkSimulator::NetworkSimulator(
    boost::asio::io_context& io,
    boost::asio::ip::udp::endpoint receiver
) : socket(io, boost::asio::ip::udp::v4()), receiver(receiver) {}

void NetworkSimulator::send(Packet packet) {

    if (shouldDropPacket()) {
        return;
    }

    auto bytes = serialize(packet);
    int delay = delayDistribution(generator);
    
    auto timer = std::make_shared<boost::asio::steady_timer>(
        socket.get_executor(),
        std::chrono::milliseconds(delay)
    );

    timer->async_wait([this, packet, bytes, delay, timer](const boost::system::error_code& error) {
        if (!error) {
            socket.send_to(
                boost::asio::buffer(bytes),
                receiver
            );
        }
    });

    std::cout << "Packet " << packet.getSequenceNumber()
          << " delayed by " << delay << " ms\n";
}

bool NetworkSimulator::shouldDropPacket() {
    return distribution(generator) == 1;
}