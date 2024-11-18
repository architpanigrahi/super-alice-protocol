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
    void startListening() override;
    void sendData(const alice::Packet &packet) override;
    void receiveData(const asio::error_code &error, std::size_t bytes_transferred) override;
};

#endif
