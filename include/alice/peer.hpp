// Created By Rokas Paulauskas on 14/11/2024
#ifndef PEER_HPP
#define PEER_HPP

#include "device_meta/device_ip_table.hpp"
#include "device_meta/eci_position_calculator.hpp"
#include "device_meta/position_table.hpp"
#include "device_meta/orbital_parameters.hpp"
#include "position_service/position_notifier.hpp"
// #include "device_meta/peer_enums.hpp"
#include "device_meta/device_type_table.hpp"
#include "packet.hpp"
#include "logger.hpp"
#include "encryption_manager.hpp"
#include <string>
#include <memory>
#include <thread>
#include <vector>

class Peer
{
public:
    Peer(const uint32_t &id, PeerType type);
    virtual ~Peer() = default;

    uint32_t getID() const;
    PeerType getType() const;

    void setHostAddress(const std::string &host_ip, uint16_t host_port);
    void setBootstrapAddress(const std::string &bootstrap_ip, uint16_t bootstrap_port);
    std::vector<uint8_t> convertIpToVector(std::string ip);
    std::vector<uint8_t> convertIpPortToIpVector(std::string ip_port);
    std::string convertIpPortVectorToIpPortString(std::vector<uint8_t> ip_vector, std::vector<uint8_t> port);
    std::string buildIp(std::vector<uint8_t> ip_vector);
    std::string getPortFromIpPort(std::string ip_port);
    void sendHandshake();
    virtual void startListening() = 0;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void sendData(const alice::Packet &packet) = 0;
    virtual void receiveData(const asio::error_code &error, std::size_t bytes_transferred) = 0;

protected:
    uint32_t id_;
    PeerType type_;
    std::string host_ip_;
    uint16_t host_port_;
    std::string bootstrap_ip_;
    uint16_t bootstrap_port_;
    asio::io_context io_context_;
    alice::EncryptionManager encryption_manager_;
    asio::ip::udp::socket socket_;
    alice::OrbitalParameters orbital_parameters_;
    alice::ECIPosition position_;
    bool listening_ = false;
    std::array<uint8_t, 1024> receive_buffer_;
    asio::ip::udp::endpoint remote_endpoint_;
    std::thread io_thread_;

    std::shared_ptr<alice::DeviceIPTable> ip_table_;
    std::shared_ptr<alice::PositionTable> position_table_;
    std::shared_ptr<alice::DeviceTypeTable> type_table_;
    std::shared_ptr<alice::PositionNotifier> position_notifier_;
};

#endif
