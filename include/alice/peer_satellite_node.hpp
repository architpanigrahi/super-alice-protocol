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
    void sendData(const alice::Packet &packet) override;
    void receiveData(const asio::error_code &error, std::size_t bytes_transferred) override;
    void startListening() override;
    void sendKeepAlive();
    void updatePosition();
    void discoverPeers();
    void startPeriodicDiscovery();
};

#endif