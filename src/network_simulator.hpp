#pragma once
#include <boost/asio.hpp>
#include "packet.hpp"

class NetworkSimulator {

private:
    boost::asio::ip::udp::socket socket;
    boost::asio::ip::udp::endpoint receiver;

    bool shouldDropPacket();

public:
    NetworkSimulator(
        boost::asio::io_context& io,
        boost::asio::ip::udp::endpoint receiver
    );

    void send(Packet packet);
};