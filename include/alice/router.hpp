//
// Created by Archit Panigrahi on 05/11/2024.
//

#ifndef ALICE_ROUTER_HPP
#define ALICE_ROUTER_HPP

#include "node.hpp"
#include "packet.hpp"
#include <cstdint>
#include <unordered_map>

namespace alice {

    class Router {
        public:
            explicit Router(Node* node);

            void updateRouting(const Packet& packet);

            void forwardPacket(const Packet& packet);

        private:
            Node* node_;
            std::unordered_map<uint32_t, uint32_t> routing_table_;
    };

}

#endif //ALICE_ROUTER_HPP
