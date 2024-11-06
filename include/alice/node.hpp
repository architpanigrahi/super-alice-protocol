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

namespace alice {

    class Node {
        public:
            Node(uint32_t id, std::string  address, uint16_t port);

            void sendPacket(const Packet& packet) const;

            void receivePacket(const std::vector<uint8_t>& data) const;

            void processPacket(const Packet& packet) const;

            static void printPayload(const std::vector<uint8_t>& payload) ;

            uint32_t getId() const { return id_; }

        private:
            uint32_t id_;
            std::string address_;
            uint16_t port_;
//            std::shared_ptr<Router> router_;
//            std::shared_ptr<ErrorHandler> error_handler_;


    };


}

#endif //ALICE_NODE_HPP
