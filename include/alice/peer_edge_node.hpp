#ifndef PEER_EDGE_NODE_HPP
#define PEER_EDGE_NODE_HPP
#include "peer.hpp"
#include <vector>
#include <string>

class PeerEdgeNode : public Peer
{
public:
    PeerEdgeNode(const uint32_t &id);
    void connect() override;
    void disconnect() override;
    void sendData(const std::vector<uint8_t> &data) override;
    void receiveData(const asio::error_code &error, std::size_t bytes_transferred) override;
    void startListening() override;
};

#endif