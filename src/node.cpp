//
// Created by Archit Panigrahi on 05/11/2024.
//


#include <alice/alice.hpp>
#include <iostream>
#include <semaphore>

namespace alice {

    Node::Node(uint32_t id, std::string  address, uint16_t port)
        : id_(id), address_(std::move(address)), port_(port) {
    }

    void Node::sendPacket(const Packet& packet) const {
        std::vector<uint8_t> data = packet.serialize();
        std::cout << "Node " << id_ << " sending packet to Node " << packet.destination_id
              << " with message type " << static_cast<int>(packet.type) << std::endl;
    }

    void Node::receivePacket(const std::vector<uint8_t>& data) const {
        Packet packet = Packet::deserialize(data);
        processPacket(packet);
    }

    void Node::processPacket(const Packet& packet) const {
        std::cout << "Node " << id_ << " processing packet from Node " << packet.source_id
                  << " with message type " << static_cast<int>(packet.type) << std::endl;

        switch (packet.type) {
            case PacketType::DATA:
                std::cout << "Processing DATA packet..." << std::endl;
                printPayload(packet.payload);
            break;
            case PacketType::ACK:
                std::cout << "Processing ACK packet..." << std::endl;
            break;
            case PacketType::NACK:
                std::cout << "Processing NACK packet..." << std::endl;
            break;
            case PacketType::CONTROL:
                std::cout << "Processing CONTROL packet..." << std::endl;
            break;
            default:
                std::cerr << "Unknown packet type received." << std::endl;
            break;
        }
    }

    void Node::printPayload(const std::vector<uint8_t>& payload) {
        std::cout << "Payload: ";
        for (const auto& byte : payload) {
            std::cout << static_cast<char>(byte);  // Convert each byte to char for display
        }
        std::cout << std::endl;
    }


}


