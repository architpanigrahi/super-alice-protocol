// Created 14/11/2024 by Rokas Paulauskas
#ifndef PEER_SATELLITE_NODE_HPP
#define PEER_SATELLITE_NODE_HPP
#include "peer.hpp"
#include <vector>
#include <string>

class PeerSatelliteNode : public Peer
{
public:
    PeerSatelliteNode(const uint32_t &id);

    void connect() override;
    void disconnect() override;

    std::pair<std::vector<uint32_t>, std::vector<uint8_t>> deserializeRoutePayload(std::vector<uint8_t> &buffer);
    std::vector<uint8_t> serializeRoutePacket(const std::vector<uint32_t> &routePayload, const std::vector<uint8_t> &dataPayload);

    void sendRouteData(const alice::Packet &packet);

    void sendData(const alice::Packet &packet) override;
    void receiveData(const asio::error_code &error, std::size_t bytes_transferred) override;
    void startListening() override;
    void sendKeepAlive();
    void updatePosition();
    void discoverPeers();
    void startPeriodicDiscovery();
    void getRoutingDetails(const alice::Packet &packet);
};

#endif