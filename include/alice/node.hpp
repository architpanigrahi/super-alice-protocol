//
// Created by Archit Panigrahi on 05/11/2024.
//

#ifndef ALICE_NODE_HPP
#define ALICE_NODE_HPP

#include "packet.hpp"
#include <cstdint>
#include <string>
#include <memory>
#include <vector>
#include "alice/encryption_manager.hpp"
#include "logger.hpp"

namespace alice {

    class Node {
        public:
            Node(uint32_t id, std::string  address, uint16_t port);

            void sendPacket(const Packet& packet);
            void receivePacket(const std::vector<uint8_t>& data);
            void handlePacket(const Packet& packet);
            void retransmitPacket(uint32_t sequence_number, uint32_t destination_id);
            void sendACK(uint32_t sequence_number, uint32_t destination_id);
            void sendNACK(uint32_t sequence_number, uint32_t destination_id);
            void processPacket(const Packet& packet);
            static void printPayload(const std::vector<uint8_t>& payload) ;

            uint32_t getId() const { return id_; }

        private:
            uint32_t id_;
            std::string address_;
            uint16_t port_;

            uint32_t next_sequence_number_;
            uint32_t expected_sequence_number_;

            std::unordered_map<uint32_t, Packet> unacknowledged_packets_;
            std::unordered_map<uint32_t, Packet> receive_buffer_;

            EncryptionManager encryptor_;

//            std::shared_ptr<Router> router_;
//            std::shared_ptr<ErrorHandler> error_handler_;


    };


}

#endif //ALICE_NODE_HPP