# Technical Specification Document

## Project Title: Super Alice - Satellite Networking Protocol  
**Document Version:** 1.0  
**Original Author:** Rokas Paulauskas  
**Co-Authors:** Ze Yan Liow, Archit Panigrahi, Sathvika Thorali  
**Date Modified:** 04/11/2024  
**Language:** C++  

---

### 1. Overview  
Super Alice is a peer-to-peer networking protocol designed for communication within Low Earth Orbit (LEO) satellite constellations. Implemented in C++, it leverages a custom UDP protocol optimized for low-latency and lightweight satellite communications. This protocol includes machine learning (ML) for optimized routing and a bootstrap node for peer discovery, enabling efficient data exchange in dynamic and constrained environments.

---

### 2. Protocol Specifications  

#### 2.1 Communication Protocols  
- **Custom UDP Protocol**: Super Alice is built on a custom UDP protocol designed for minimal latency and efficient data transmission in satellite networks. This choice balances speed and simplicity, critical for high-frequency communication in LEO constellations.

![Custom UDP Protocol Flow](https://i.imgur.com/RWAAazL.png "Custom UDP Protocol Flow")

#### 2.2 Enhanced Packet Structure  

The Super Alice protocol has a modified packet structure to improve flexibility, reliability, and future extensibility. The structure is defined as follows:

```cpp
#ifndef ALICE_PACKET_HPP
#define ALICE_PACKET_HPP

#include <cstdint>
#include <vector>

namespace alice {

   enum class PacketType : uint8_t {
       HANDSHAKE,
       DATA,
       ACK,
       NACK,
       CONTROL,
       ERROR
   };

   class Packet {
   public:
       uint32_t source_id;
       uint32_t destination_id;
       PacketType type;
       uint8_t priority;
       uint32_t sequence_number;
       uint64_t timestamp;
       uint16_t fragment_id;
       uint16_t total_fragments;
       uint16_t crc;
       uint16_t reserved;
       uint16_t payload_type;
       std::vector<uint8_t> payload;

       Packet(uint32_t source_id, uint32_t destination_id, PacketType type, uint8_t priority, uint32_t sequence_number, const std::vector<uint8_t> &payload);

       [[nodiscard]] std::vector<uint8_t> serialize() const;
       static Packet deserialize(const std::vector<uint8_t> &buffer);
   };
}

#endif
```

---

### 3. Field Descriptions  

1. **Source ID (`source_id`)**: 32-bit identifier unique to the originating satellite or ground node.  
2. **Destination ID (`destination_id`)**: 32-bit identifier unique to the target node, ensuring directed communication.  
3. **Packet Type (`type`)**: Enum for packet type:
   - **HANDSHAKE**: Initializes communication between nodes.
   - **DATA**: Main data transfer packet.
   - **ACK/NACK**: Acknowledgments for successful or failed delivery.
   - **CONTROL**: Command packet for network adjustments.
   - **ERROR**: Reports errors to aid in diagnostics.
4. **Priority (`priority`)**: 8-bit field indicating the priority of the packet (0 = low, 255 = high).  
5. **Sequence Number (`sequence_number`)**: 32-bit integer for order in packet transmission.  
6. **Timestamp (`timestamp`)**: 64-bit Unix timestamp for packet timing.  
7. **Fragment ID (`fragment_id`)**: 16-bit identifier for packets that are part of a larger message.  
8. **Total Fragments (`total_fragments`)**: 16-bit field indicating the total number of fragments for reassembly.  
9. **CRC (`crc`)**: 16-bit error correction code for data integrity.  
10. **Reserved (`reserved`)**: 16-bit field for future enhancements.  
11. **Payload Type (`payload_type`)**: 16-bit field to specify the type of payload (e.g., text, binary).  
12. **Payload (`payload`)**: Variable-length data section containing the main content.

---

### 4. Enhanced Features  

#### 4.1 Machine Learning for Routing Optimization  
- **Purpose**: Dynamically adapt routes based on network data, improving latency and throughput.  
- **Functionality**: ML algorithms analyze traffic, node availability, and link quality, adjusting routes as conditions evolve.  
- **Data**: Training data includes positional, latency, and throughput metrics, retrained periodically as new data accumulates.

![Routing with Machine Learning](https://i.imgur.com/NPbx0GU.png "Routing with Machine Learning")

#### 4.2 Bootstrap Node for Peer Discovery  
- **Purpose**: Streamlines network initialization, connecting new nodes to nearby peers.  
- **Functionality**: The bootstrap node serves as an initial registry for nodes, allowing them to efficiently discover peers and connect.

![Peer Discovery Process](https://i.imgur.com/Q029UGM.png "Peer Discovery Process")

---

### 5. Protocol Operation  

#### 5.1 Handshake Mechanism  
- Uses the **HANDSHAKE** packet to initiate communication. The bootstrap node aids by introducing nodes to the network.

#### 5.2 Reliable Data Transfer  
- **DATA** packets carry core messages.  
- **ACK/NACK** packets ensure delivery reliability.

#### 5.3 Control Operations  
- The **CONTROL** packet enables real-time adjustments for network configuration and ML-based route updates.

#### 5.4 Error Handling  
- **ERROR** packets report any network issues, allowing rerouting or troubleshooting.

---

### 6. Routing and Connection Management  

- **ML-Optimized Routing**: ML-driven path predictions enhance data throughput and reduce latency.  
- **Direct Satellite Routing**: Satellite-to-satellite transfers leverage ISLs based on ML-optimized paths.  
- **Node Failure Resilience**: The bootstrap node assists in re-establishing connections when a node fails.  
- **Load Balancing**: Adjusts data flow according to network demand, prioritizing CONTROL and ACK/NACK packets.

---

### 7. Security and Encryption  
Super Alice implements payload encryption, especially for CONTROL and DATA packets, to secure transmissions. Future updates may integrate adaptive ML-driven security mechanisms.

---

### 8. Future Considerations  

1. **Enhanced Learning Models**: Implement reinforcement learning for more adaptive routing.  
2. **Advanced Error Correction**: Integrate forward error correction for greater robustness.  
3. **Protocol Switching**: Explore the potential for adaptive protocol switching within UDP, leveraging ML to dynamically adjust configurations based on network needs.
