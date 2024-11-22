// Created By Rokas Paulauskas on 14/11/2024

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
    std::vector<uint8_t> serializeIpTable(uint32_t id);
    void sendData(const alice::Packet &packet) override;

    [[nodiscard]] std::vector<uint8_t> serializeRoutePacket(const std::vector<uint32_t> &optimal_route, std::vector<uint8_t> dataPayload);

    uint32_t greedyForwarding(uint32_t source_id, std::vector<uint32_t> optimal_route);

    void receiveData(const asio::error_code &error, std::size_t bytes_transferred) override;
};

#endif
