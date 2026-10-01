cmake_minimum_required(VERSION 3.16)

project(udp_streamer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)

add_executable(sender
    src/sender.cpp
    src/packet.cpp
    src/network_simulator.cpp
)
add_executable(receiver
    src/receiver.cpp
    src/packet.cpp
    src/jitter_buffer.cpp
)
add_executable(packet_test
    tests/packet_test.cpp
    src/packet.cpp
)
add_executable(jitter_buffer_test
    tests/jitter_buffer_test.cpp
    src/packet.cpp
    src/jitter_buffer.cpp
)
target_link_libraries(sender PRIVATE Boost::system)
target_link_libraries(receiver PRIVATE Boost::system)