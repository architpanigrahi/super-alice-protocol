#ifndef PEER_BOOTSTRAP_NODE_HPP
#define PEER_BOOTSTRAP_NODE_HPP

#include "peer.hpp"
#include <vector>
#include <string>

class PeerBootstrapNode : public Peer
{
public:
    PeerBootstrapNode(const uint32_t &id);

    void connect() override;
    void disconnect() override;
    void sendData(const std::vector<uint8_t> &data) override;
    void receiveData(std::vector<uint8_t> &data) override;
};

#endif
