//
// Created by Archit Panigrahi on 05/11/2024.
//


#include <alice/alice.hpp>
#include <iostream>
#include <semaphore>

namespace alice {
    EncryptionManager encryption_obj;

    Node::Node(uint32_t id, std::string address, uint16_t port)
        : id_(id), address_(std::move(address)), port_(port),
        next_sequence_number_(0), expected_sequence_number_(0) {
    }

    void Node::sendPacket(const Packet& packet){
        Packet packet_to_send = packet;
        packet_to_send.sequence_number = next_sequence_number_++;
        packet_to_send.timestamp = static_cast<uint64_t>(std::time(nullptr));

        std::vector<uint8_t> data = packet_to_send.serialize(encryptor_);

        unacknowledged_packets_.emplace(packet_to_send.sequence_number, packet_to_send);

        Logger::log(LogLevel::INFO, "Node " + std::to_string(id_) + " sent packet with sequence number " +
            std::to_string(packet_to_send.sequence_number) + " to Node " +
            std::to_string(packet_to_send.destination_id));
    }

    void Node::receivePacket(const std::vector<uint8_t>& data){
        try {
            Packet packet = Packet::deserialize(data, encryptor_);
            processPacket(packet);
        } catch (const std::exception& e) {
            Logger::log(LogLevel::ERROR, "Node " + std::to_string(id_) + " failed to deserialize packet: " + e.what());
        }
    }

    void Node::processPacket(const Packet& packet){
        if (packet.sequence_number == expected_sequence_number_) {
            expected_sequence_number_++;
            handlePacket(packet);

            while (true) {
                auto it = receive_buffer_.find(expected_sequence_number_);
                if (it == receive_buffer_.end()) {
                    break;
                }

                Packet next_packet = it->second;
                receive_buffer_.erase(it);
                expected_sequence_number_++;
                handlePacket(next_packet);
            }

            sendACK(packet.sequence_number, packet.source_id);

        } else if (packet.sequence_number > expected_sequence_number_) {
            receive_buffer_.emplace(packet.sequence_number, packet);

            for (uint32_t seq = expected_sequence_number_; seq < packet.sequence_number; ++seq) {
                sendNACK(seq, packet.source_id);
            }

        } else {
            sendACK(packet.sequence_number, packet.source_id);
        }
    }

    void Node::handlePacket(const Packet& packet){
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
                Logger::log(LogLevel::INFO, "Processing CONTROL packet...");
            break;            
            default:
                Logger::log(LogLevel::ERROR, "Node " + std::to_string(id_) + " received unknown packet type.");
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
        auto it = unacknowledged_packets_.find(sequence_number);
        if (it != unacknowledged_packets_.end()) {
            Packet packet_to_retransmit = it->second;
            packet_to_retransmit.timestamp = static_cast<uint64_t>(std::time(nullptr));
            std::vector<uint8_t> data = packet_to_retransmit.serialize(encryptor_);

            Logger::log(LogLevel::INFO, "Node " + std::to_string(id_) + " retransmitted packet with sequence number " +
            std::to_string(sequence_number) + " to Node " + std::to_string(destination_id));

        } else {
            Logger::log(LogLevel::ERROR, "Node " + std::to_string(id_) + " has no record of packet with sequence number " +
            std::to_string(sequence_number) + " to retransmit.");
        }
    }

    void Node::printPayload(const std::vector<uint8_t>& payload) {
        std::string payload_str(payload.begin(), payload.end());
        Logger::log(LogLevel::INFO, "Payload: " + payload_str);
    }
}


