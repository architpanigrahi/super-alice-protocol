#include <alice/alice.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>


void simulateNetworkDelay() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int main()
{
    alice::Node node1(1, "127.0.0.1", 8081);
    alice::Node node2(2, "127.0.0.1", 8082);
    alice::Node node3(3, "127.0.0.1", 8083);

    // Step 1: Set up a route from node1 to node3 via node2 using CONTROL packets
    std::vector<uint8_t> controlPayload1;
    uint32_t destination_id = 3;
    uint32_t next_hop_id = 2;
    controlPayload1.insert(controlPayload1.end(), reinterpret_cast<uint8_t*>(&destination_id), reinterpret_cast<uint8_t*>(&destination_id) + sizeof(destination_id));
    controlPayload1.insert(controlPayload1.end(), reinterpret_cast<uint8_t*>(&next_hop_id), reinterpret_cast<uint8_t*>(&next_hop_id) + sizeof(next_hop_id));

    alice::Packet controlPacket1(1, 2, alice::PacketType::CONTROL, 1, controlPayload1);
    node2.receivePacket(controlPacket1.serialize());  // node2 receives the routing update

    // Step 2: Set up a route from node2 to node3 directly
    std::vector<uint8_t> controlPayload2;
    next_hop_id = 3;
    controlPayload2.insert(controlPayload2.end(), reinterpret_cast<uint8_t*>(&destination_id), reinterpret_cast<uint8_t*>(&destination_id) + sizeof(destination_id));
    controlPayload2.insert(controlPayload2.end(), reinterpret_cast<uint8_t*>(&next_hop_id), reinterpret_cast<uint8_t*>(&next_hop_id) + sizeof(next_hop_id));

    alice::Packet controlPacket2(2, 3, alice::PacketType::CONTROL, 2, controlPayload2);
    node3.receivePacket(controlPacket2.serialize());  // node3 receives the routing update

    // Step 3: Send a DATA packet from node1 to node3, which should route through node2
    std::vector<uint8_t> dataPayload = {'H', 'e', 'l', 'l', 'o'};
    alice::Packet dataPacket(1, 3, alice::PacketType::DATA, 3, dataPayload);

    std::cout << "\nSending DATA packet from node1 to node3...\n";
    node1.sendPacket(dataPacket);
    simulateNetworkDelay();
    node2.receivePacket(dataPacket.serialize());  // node2 receives the packet
    simulateNetworkDelay();
    node3.receivePacket(dataPacket.serialize());  // node3 receives the packet

    // Step 4: Simulate packet loss by sending a DATA packet to a non-existing route
    std::cout << "\nTesting Error Handling by sending packet to a non-routable node...\n";
    alice::Packet lostPacket(1, 4, alice::PacketType::DATA, 4, dataPayload); // Node 4 does not exist
    node1.sendPacket(lostPacket);
    simulateNetworkDelay();
    node2.receivePacket(lostPacket.serialize());  // This should trigger the error handler for no route found

    return 0;
}
