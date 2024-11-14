#ifndef PEER_HPP
#define PEER_HPP

#include "device_meta/device_ip_table.hpp"
#include "device_meta/eci_position_calculator.hpp"
#include "device_meta/position_table.hpp"
#include "device_meta/orbital_parameters.hpp"
#include "position_service/position_notifier.hpp"
#include "logger.hpp"
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
    Peer(const uint32_t &id, PeerType type);
    virtual ~Peer() = default;

    uint32_t getID() const;
    PeerType getType() const;

    void setHostAddress(const std::string &host_ip, uint16_t host_port);
    void setBootstrapAddress(const std::string &bootstrap_ip, uint16_t bootstrap_port);

    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void sendData(const std::vector<uint8_t> &data) = 0;
    virtual void receiveData(std::vector<uint8_t> &data) = 0;

protected:
    uint32_t id_;
    PeerType type_;
    std::string host_ip_;
    uint16_t host_port_;
    std::string bootstrap_ip_;
    uint16_t bootstrap_port_;
    asio::io_context io_context_;

    std::shared_ptr<alice::DeviceIPTable> ip_table_;
    std::shared_ptr<alice::ECIPositionCalculator> position_calculator_;
    std::shared_ptr<alice::PositionTable> position_table_;
    std::shared_ptr<alice::OrbitalParameters> orbital_params_;
    std::shared_ptr<alice::PositionNotifier> position_notifier_;
};

#endif
