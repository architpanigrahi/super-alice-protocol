//
// Created by Archit Panigrahi on 05/11/2024.
//

#ifndef ALICE_ERROR_HANDLER_HPP
#define ALICE_ERROR_HANDLER_HPP

#include "node.hpp"
#include <iostream>
#include <string>

namespace alice {

    class ErrorHandler {
        public:
            explicit ErrorHandler(Node* node);

            void handlePacketLoss(uint32_t sequence_number);
            void handleInvalidRoute(uint32_t destination_id);
            void handleTimeout(const std::string& context);

    private:
        Node* node_;
    };

}

#endif //ALICE_ERROR_HANDLER_HPP
