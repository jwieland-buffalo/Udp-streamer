
#include "packet.hpp"
#include "jitter_buffer.hpp"

#include <boost/asio.hpp>

#include <array>
#include <chrono>
#include <cstdint>
#include <exception>
#include <iostream>
#include <vector>

using boost::asio::ip::udp;
using namespace std::chrono_literals;

class Receiver {
private:
    static constexpr std::uint32_t last_sequence = 99;

    boost::asio::io_context io;
    udp::socket socket;
    udp::endpoint sender;
    std::array<std::uint8_t, 1024> receive_buffer{};

    JitterBuffer jitter_buffer{0};

    boost::asio::steady_timer startup_timer{io};
    boost::asio::steady_timer playback_timer{io};

    bool startup_started = false;
    bool playing = false;
    bool finished = false;

    std::size_t received_packets = 0;
    std::size_t played_packets = 0;
    std::size_t skipped_packets = 0;

    void receive_packet() {
        if (finished) {
            return;
        }

        socket.async_receive_from(
            boost::asio::buffer(receive_buffer),
            sender,
            [this](const boost::system::error_code& error,
                   std::size_t bytes) {
                if (finished) {
                    return;
                }

                if (error) {
                    if (error != boost::asio::error::operation_aborted) {
                        std::cerr << "Receive error: "
                                  << error.message() << '\n';
                    }
                    return;
                }

                if (bytes != 172) {
                    std::cerr << "Unexpected packet size: "
                              << bytes << '\n';
                } else {
                    try {
                        std::vector<std::uint8_t> data(
                            receive_buffer.begin(),
                            receive_buffer.begin() + bytes
                        );

                        Packet packet = deserialize(data);
                        auto sequence = packet.getSequenceNumber();

                        if (sequence <= last_sequence) {
                            jitter_buffer.insert(packet);
                            ++received_packets;

                            std::cout << "Received packet: "
                                      << sequence << '\n';

                            // Give the network time to deliver
                            // reordered packets before playback starts.
                            if (!startup_started) {
                                startup_started = true;

                                startup_timer.expires_after(150ms);
                                startup_timer.async_wait(
                                    [this](const boost::system::error_code& e) {
                                        if (e || finished) {
                                            return;
                                        }

                                        playing = true;
                                        std::cout << "Starting playback...\n";
                                        schedule_playback();
                                    }
                                );
                            }
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Invalid packet: "
                                  << e.what() << '\n';
                    }
                }

                receive_packet();
            }
        );
    }

    void schedule_playback() {
        if (finished) {
            return;
        }

        playback_timer.expires_after(20ms);

        playback_timer.async_wait(
            [this](const boost::system::error_code& error) {
                if (error || finished || !playing) {
                    return;
                }

                auto expected = jitter_buffer.expectedSequence();

                if (expected > last_sequence) {
                    finish();
                    return;
                }

                auto packet = jitter_buffer.popNext();

                if (packet.has_value()) {
                    ++played_packets;

                    std::cout << "Played packet: "
                              << packet->getSequenceNumber() << '\n';
                } else {
                    ++skipped_packets;

                    std::cout << "Missing packet " << expected
                              << "; skipping at playback deadline\n";

                    jitter_buffer.skipExpected();
                }

                if (jitter_buffer.expectedSequence() > last_sequence) {
                    finish();
                    return;
                }

                schedule_playback();
            }
        );
    }

    void finish() {
        if (finished) {
            return;
        }

        finished = true;

        boost::system::error_code ignored;
        startup_timer.cancel(ignored);
        playback_timer.cancel(ignored);
        socket.close(ignored);

        std::cout << "\nPlayback finished.\n"
                  << "Packets received: " << received_packets << '\n'
                  << "Packets played:   " << played_packets << '\n'
                  << "Packets skipped:  " << skipped_packets << '\n';

        io.stop();
    }

public:
    Receiver()
        : socket(io, udp::endpoint(udp::v4(), 9000))
    {
    }

    void run() {
        std::cout << "Waiting for packets on port 9000...\n";

        receive_packet();
        io.run();
    }
};

int main() {
    try {
        Receiver receiver;
        receiver.run();
    } catch (const std::exception& e) {
        std::cerr << "Receiver failed: " << e.what() << '\n';
        return 1;
    }

    return 0;
}