# UDP Streamer

A small C++20 project that simulates a real-time packet stream over UDP. A sender pushes numbered packets through a simulated bad network (random loss and delay), and a receiver uses a jitter buffer to put them back in order and play them out on a steady schedule.

I built it to get hands-on with the tools and ideas behind real-time voice systems: UDP, async I/O with Boost.Asio, reordering, loss handling, and unit testing in modern C++.

## How it works

**Sender** (`sender.cpp`, `network_simulator.cpp`)
Creates 100 packets and sends them through a `NetworkSimulator`. The simulator drops about 1 in 20 packets and delays each remaining packet by a random 0 to 100 ms, which also causes packets to arrive out of order.

**Receiver** (`receiver.cpp`, `jitter_buffer.cpp`)
Listens on UDP port 9000 and inserts each valid packet into the jitter buffer. After the first packet arrives it waits 150 ms so late and reordered packets have time to show up, then plays one packet every 20 ms. If the next expected packet is missing at its deadline, it is skipped and counted. When the last packet's slot has passed, the receiver prints stats and exits.

**Packet format** (`packet.cpp`)
Each packet is 172 bytes on the wire, with every field written byte by byte in big-endian order:

| Field           | Size      |
|-----------------|-----------|
| Sequence number | 4 bytes   |
| Timestamp (ms)  | 8 bytes   |
| Payload         | 160 bytes |

The 160-byte payload matches 20 ms of 8 kHz, 8-bit audio, which lines up with the 20 ms playback tick. The payload is dummy data here.

## Design notes

- **Why UDP:** For live audio, a late packet is useless, so waiting on retransmission (as TCP would) hurts more than it helps. Loss is handled at the application level instead.
- **Why `std::map` for the buffer:** Keyed by sequence number, it keeps packets ordered automatically, with O(log n) insert, lookup, and erase.
- **Single-threaded:** Everything runs on one `boost::asio::io_context`, so all handlers run on one thread and the jitter buffer needs no locking.
- **Late packets:** Anything that arrives after its sequence number has already been played or skipped is ignored.
- **Startup delay:** The 150 ms delay trades a little latency for tolerance to jitter. A bigger delay means fewer late packets but a longer wait.

## Building

Requires a C++17 compiler, CMake 3.16 or newer, and Boost. On Ubuntu or Debian:

```
sudo apt install build-essential cmake libboost-all-dev
```

Then:

```
mkdir build
cd build
cmake ..
cmake --build .
```

## Running

Start the receiver first, then the sender from a second terminal (both from the `build` folder):

```
./receiver
./sender
```

The receiver prints each packet it gets, each packet it plays, and any packet it skips, then ends with a summary of packets received, played, and skipped.

Example output:

```
(paste a real run here)
```

## Tests

Unit tests cover packet serialization and the jitter buffer. Run them from the `build` folder:

```
./packet_test
./jitter_buffer_test
```

## Known limitations and next steps

This is a learning project, and there are things a production version would need:

- The receiver is hard-coded for a 100-packet run (last sequence number is 99), not an open-ended stream.
- The sender sends all packets at once instead of one every 20 ms like real audio would.
- The timestamp is sent but not used yet. The next step would be computing jitter from the difference between send spacing and arrival spacing.
- Sequence numbers are 32-bit and the buffer does not handle wraparound.
- The jitter buffer has a fixed delay and no size limit. A real one would adapt to network conditions.
- Missing packets are just skipped. There is no loss concealment.
- Stats do not separate packets lost by the network from packets that arrived too late.
- Ports and addresses are hard-coded.
