#include "alice/peer.hpp"

void Peer::setHostAddress(const std::string &host_ip, uint16_t host_port)
{
    host_ip_ = host_ip;
    host_port_ = host_port;
}
void Peer::setBootstrapAddress(const std::string &bootstrap_ip, uint16_t bootstrap_port)
{
    bootstrap_ip_ = bootstrap_ip;
    bootstrap_port_ = bootstrap_port;
}
uint32_t Peer::getID() const
{
    return id_;
}
PeerType Peer::getType() const
{
    return type_;
}
Peer::Peer(const uint32_t &id, PeerType type)
    : id_(id), type_(type), ip_table_(std::make_shared<alice::DeviceIPTable>()), position_table_(std::make_shared<alice::PositionTable>()), position_notifier_(std::make_shared<alice::PositionNotifier>(io_context_, 0, 0, 0, 0)), socket_(io_context_, asio::ip::udp::v4()), encryption_manager_(alice::EncryptionManager())
{
}