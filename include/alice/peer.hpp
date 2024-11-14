#ifndef PEER_HPP
#define PEER_HPP

#include "device_meta/device_ip_table.hpp"
#include "device_meta/eci_position_calculator.hpp"
#include "device_meta/position_table.hpp"
#include "device_meta/orbital_parameters.hpp"
#include "position_service/position_notifier.hpp"
#include <string>
#include <memory>
#include <vector>

enum class PeerType
{
    BOOTSTRAP_NODE,
    SATELLITE,
    EDGE_DEVICE
};

class Peer
{
public:
    Peer(const std::string &id, PeerType type);
    virtual ~Peer() = default;

    std::string getID() const;
    PeerType getType() const;

    void setHostAddress(const std::string &host_ip, int host_port);
    void setBootstrapAddress(const std::string &bootstrap_ip, int bootstrap_port);

    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void sendData(const std::vector<uint8_t> &data) = 0;
    virtual void receiveData(std::vector<uint8_t> &data) = 0;

private:
    std::string id_;
    PeerType type_;
    std::string host_ip_;
    int host_port_;
    std::string bootstrap_ip_;
    int bootstrap_port_;

    std::shared_ptr<DeviceIPTable> ip_table_;
    std::shared_ptr<ECIPositionCalculator> position_calculator_;
    std::shared_ptr<PositionTable> position_table_;
    std::shared_ptr<OrbitalParameters> orbital_params_;
    std::shared_ptr<PositionNotifier> position_notifier_;
};

std::unique_ptr<Peer> createPeer(const std::string &id, PeerType type);

#endif
