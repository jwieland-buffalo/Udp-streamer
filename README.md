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
./build/receiver
Waiting for packets on port 9000...
Received packet: 9
Received packet: 68
Received packet: 67
Received packet: 56
Received packet: 29
Received packet: 32
Received packet: 59
Received packet: 31
Received packet: 70
Received packet: 87
Received packet: 95
Received packet: 54
Received packet: 96
Received packet: 85
Received packet: 99
Received packet: 88
Received packet: 89
Received packet: 3
Received packet: 1
Received packet: 50
Received packet: 71
Received packet: 40
Received packet: 44
Received packet: 46
Received packet: 78
Received packet: 90
Received packet: 33
Received packet: 45
Received packet: 65
Received packet: 26
Received packet: 91
Received packet: 8
Received packet: 19
Received packet: 94
Received packet: 15
Received packet: 30
Received packet: 69
Received packet: 16
Received packet: 5
Received packet: 7
Received packet: 23
Received packet: 55
Received packet: 25
Received packet: 52
Received packet: 53
Received packet: 27
Received packet: 97
Received packet: 18
Received packet: 57
Received packet: 28
Received packet: 64
Received packet: 61
Received packet: 74
Received packet: 82
Received packet: 43
Received packet: 47
Received packet: 92
Received packet: 13
Received packet: 20
Received packet: 36
Received packet: 63
Received packet: 98
Received packet: 10
Received packet: 38
Received packet: 2
Received packet: 37
Received packet: 58
Received packet: 73
Received packet: 34
Received packet: 48
Received packet: 81
Received packet: 49
Received packet: 11
Received packet: 4
Received packet: 35
Received packet: 17
Received packet: 60
Received packet: 76
Received packet: 84
Received packet: 86
Received packet: 12
Received packet: 42
Received packet: 14
Received packet: 24
Received packet: 80
Received packet: 72
Received packet: 22
Received packet: 51
Received packet: 93
Received packet: 0
Received packet: 39
Received packet: 79
Received packet: 75
Received packet: 77
Received packet: 66
Received packet: 21
Received packet: 83
Starting playback...
Played packet: 0
Played packet: 1
Played packet: 2
Played packet: 3
Played packet: 4
Played packet: 5
Missing packet 6; skipping at playback deadline
Played packet: 7
Played packet: 8
Played packet: 9
Played packet: 10
Played packet: 11
Played packet: 12
Played packet: 13
Played packet: 14
Played packet: 15
Played packet: 16
Played packet: 17
Played packet: 18
Played packet: 19
Played packet: 20
Played packet: 21
Played packet: 22
Played packet: 23
Played packet: 24
Played packet: 25
Played packet: 26
Played packet: 27
Played packet: 28
Played packet: 29
Played packet: 30
Played packet: 31
Played packet: 32
Played packet: 33
Played packet: 34
Played packet: 35
Played packet: 36
Played packet: 37
Played packet: 38
Played packet: 39
Played packet: 40
Missing packet 41; skipping at playback deadline
Played packet: 42
Played packet: 43
Played packet: 44
Played packet: 45
Played packet: 46
Played packet: 47
Played packet: 48
Played packet: 49
Played packet: 50
Played packet: 51
Played packet: 52
Played packet: 53
Played packet: 54
Played packet: 55
Played packet: 56
Played packet: 57
Played packet: 58
Played packet: 59
Played packet: 60
Played packet: 61
Missing packet 62; skipping at playback deadline
Played packet: 63
Played packet: 64
Played packet: 65
Played packet: 66
Played packet: 67
Played packet: 68
Played packet: 69
Played packet: 70
Played packet: 71
Played packet: 72
Played packet: 73
Played packet: 74
Played packet: 75
Played packet: 76
Played packet: 77
Played packet: 78
Played packet: 79
Played packet: 80
Played packet: 81
Played packet: 82
Played packet: 83
Played packet: 84
Played packet: 85
Played packet: 86
Played packet: 87
Played packet: 88
Played packet: 89
Played packet: 90
Played packet: 91
Played packet: 92
Played packet: 93
Played packet: 94
Played packet: 95
Played packet: 96
Played packet: 97
Played packet: 98
Played packet: 99

Playback finished.
Packets received: 97
Packets played:   97
Packets skipped:  3
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
