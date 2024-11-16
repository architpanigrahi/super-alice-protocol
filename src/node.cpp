//
// Created by Archit Panigrahi on 05/11/2024.
//


#include <alice/alice.hpp>
#include <iostream>
#include <semaphore>

namespace alice {

    Node::Node(uint32_t id, std::string address, uint16_t port)
        : id_(id), address_(std::move(address)), port_(port),
        next_sequence_number_(0), expected_sequence_number_(0) {
    }

    void Node::sendPacket(const Packet& packet) const {
        std::vector<uint8_t> data = packet.serialize();
        std::cout << "Node " << id_ << " sending packet to Node " << packet.destination_id
              << " with message type " << static_cast<int>(packet.type) << std::endl;
    }

    void Node::sendPacket(const Packet& packet) {
        Packet packet_to_send = packet;
        packet_to_send.sequence_number = next_sequence_number_++;
        packet_to_send.timestamp = static_cast<uint64_t>(std::time(nullptr));

        std::vector<uint8_t> data = packet_to_send.serialize(encryptor_);

        unacknowledged_packets_[packet_to_send.sequence_number] = packet_to_send;

        std::cout << "Node " << id_ << " sent packet with sequence number "
                << packet_to_send.sequence_number << " to Node "
                << packet_to_send.destination_id << std::endl;
    }

    void Node::receivePacket(const std::vector<uint8_t>& data) {
        try {
            Packet packet = Packet::deserialize(data, encryptor_);
            processPacket(packet);
        } catch (const std::exception& e) {
            std::cerr << "Node " << id_ << " failed to deserialize packet: " << e.what() << std::endl;
        }
    }

    void Node::processPacket(const Packet& packet) {
        if (packet.sequence_number == expected_sequence_number_) {
            expected_sequence_number_++;
            handlePacket(packet);

            while (receive_buffer_.count(expected_sequence_number_)) {
                Packet next_packet = receive_buffer_[expected_sequence_number_];
                receive_buffer_.erase(expected_sequence_number_);
                expected_sequence_number_++;
                handlePacket(next_packet);
            }

            sendACK(packet.sequence_number, packet.source_id);

        } else if (packet.sequence_number > expected_sequence_number_) {
            receive_buffer_[packet.sequence_number] = packet;

            for (uint32_t seq = expected_sequence_number_; seq < packet.sequence_number; ++seq) {
                sendNACK(seq, packet.source_id);
            }

        } else {
            sendACK(packet.sequence_number, packet.source_id);
        }
    }

    void Node::handlePacket(const Packet& packet) {
        switch (packet.type) {
            case PacketType::DATA:
                printPayload(packet.payload);
                break;
            case PacketType::ACK:
                unacknowledged_packets_.erase(packet.sequence_number);
                break;
            case PacketType::NACK:
                retransmitPacket(packet.sequence_number, packet.source_id);
                break;
            case PacketType::CONTROL:
                std::cout << "Processing CONTROL packet..." << std::endl;
            break;            
            default:
                std::cerr << "Node " << id_ << " received unknown packet type." << std::endl;
            break;
        }
    }

    void Node::sendACK(uint32_t sequence_number, uint32_t destination_id) {
        Packet ack_packet(id_, destination_id, PacketType::ACK, /* priority */ 0,
                        sequence_number, {}, /* crc */ 0);
        std::vector<uint8_t> data = ack_packet.serialize(encryptor_);
        //TODO: implement sending logic
    }

    void Node::sendNACK(uint32_t sequence_number, uint32_t destination_id) {
        Packet nack_packet(id_, destination_id, PacketType::NACK, /* priority */ 0,
                        sequence_number, {}, /* crc */ 0);
        std::vector<uint8_t> data = nack_packet.serialize(encryptor_);
        //TODO: implement sending logic
    }

    void Node::retransmitPacket(uint32_t sequence_number, uint32_t destination_id) {
        if (unacknowledged_packets_.count(sequence_number)) {
            Packet packet_to_retransmit = unacknowledged_packets_[sequence_number];
            packet_to_retransmit.timestamp = static_cast<uint64_t>(std::time(nullptr));
            std::vector<uint8_t> data = packet_to_retransmit.serialize(encryptor_);
            std::cout << "Node " << id_ << " retransmitted packet with sequence number "
                    << sequence_number << " to Node " << destination_id << std::endl;
        } else {
            std::cerr << "Node " << id_ << " has no record of packet with sequence number "
                    << sequence_number << " to retransmit." << std::endl;
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


