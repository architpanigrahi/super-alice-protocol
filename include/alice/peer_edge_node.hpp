// Created 14/11/2024 by Rokas Paulauskas

#ifndef PEER_EDGE_NODE_HPP
#define PEER_EDGE_NODE_HPP

#include "peer.hpp"
#include "logger.hpp"
#include "packet.hpp"
#include <asio.hpp>
#include <thread>
#include <vector>
#include <string>

class PeerEdgeNode : public Peer
{
public:
    PeerEdgeNode(const uint32_t &id);
    void connect() override;
    void disconnect() override;
    void sendData(const alice::Packet &packet) override;
    void receiveData(const asio::error_code &error, std::size_t bytes_transferred) override;
    void startListening() override;
    void sendHandshake();

private:
    double latitude_;  // Latitude of the edge node
    double longitude_; // Longitude of the edge node
    double altitude_;  // Altitude of the edge node in meters
};

#endif
