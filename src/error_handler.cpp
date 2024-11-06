//
// Created by Archit Panigrahi on 05/11/2024.
//

#include <alice/alice.hpp>
#include <iostream>

namespace alice {

    ErrorHandler::ErrorHandler(Node* node) : node_(node) {}

    void ErrorHandler::handlePacketLoss(uint32_t sequence_number) {
        std::cout << "ErrorHandler for Node " << node_->getId()
                  << ": Detected packet loss for sequence number " << sequence_number << std::endl;
    }

    void ErrorHandler::handleInvalidRoute(uint32_t destination_id) {
        std::cerr << "ErrorHandler for Node " << node_->getId()
                  << ": No route found for destination " << destination_id << std::endl;
    }

    void ErrorHandler::handleTimeout(const std::string& context) {
        std::cerr << "ErrorHandler for Node " << node_->getId()
                  << ": Operation timeout in context '" << context << "'" << std::endl;
    }

}
