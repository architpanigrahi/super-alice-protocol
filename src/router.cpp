//
// Created by Archit Panigrahi on 05/11/2024.
//

#include <alice/alice.hpp>
#include <iostream>
#include <cstring>

namespace alice {

    Router::Router(Node* node) : node_(node) {}

    void Router::updateRouting(const Packet& packet) {
        if (packet.payload.size() >= 2 * sizeof(uint32_t)) {
            uint32_t destination_id;
            uint32_t next_hop_id;

            std::memcpy(&destination_id, packet.payload.data(), sizeof(destination_id));
            std::memcpy(&next_hop_id, packet.payload.data() + sizeof(destination_id), sizeof(next_hop_id));

            routing_table_[destination_id] = next_hop_id;

            std::cout << "Router for Node " << node_->getId() << " updated route: "
                      << "Destination " << destination_id << " -> Next Hop " << next_hop_id << std::endl;
        } else {
            std::cerr << "Invalid CONTROL packet payload for routing update." << std::endl;
        }
    }

    void Router::forwardPacket(const Packet& packet) {
        auto next_hop_it = routing_table_.find(packet.destination_id);
        if (next_hop_it != routing_table_.end()) {
            uint32_t next_hop_id = next_hop_it->second;

            std::cout << "Router for Node " << node_->getId() << " forwarding packet to Next Hop " << next_hop_id << std::endl;

            Packet forward_packet = packet;
            forward_packet.source_id = node_->getId();

            node_->sendPacket(forward_packet);
        } else {
            std::cerr << "No route found for destination " << packet.destination_id << std::endl;
        }
    }


}