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
std::string Peer::getPortFromIpPort(std::string ip_port)
{
    std::istringstream ip_stream(ip_port);
    std::string ip_part;
    std::getline(ip_stream, ip_part, ':');
    std::getline(ip_stream, ip_part);
    return ip_part;
}
std::string Peer::buildIp(std::vector<uint8_t> ip_vector)
{
    std::string ip;
    for (size_t i = 0; i < ip_vector.size(); i++)
    {
        ip += std::to_string(ip_vector[i]);
        if (i < ip_vector.size() - 1)
        {
            ip += ".";
        }
    }
    return ip;
}
std::vector<uint8_t> Peer::convertIpToVector(std::string ip)
{
    std::vector<uint8_t> ip_vector;
    std::istringstream ip_stream(ip);
    std::string ip_part;
    while (std::getline(ip_stream, ip_part, '.'))
    {
        ip_vector.push_back(std::stoi(ip_part));
    }
    return ip_vector;
}
std::vector<uint8_t> Peer::convertIpPortToIpVector(std::string ip_port)
{
    std::istringstream ip_stream(ip_port);
    std::string ip_part;
    std::getline(ip_stream, ip_part, ':');
    std::vector<uint8_t> ip_vector = convertIpToVector(ip_part);
    return ip_vector;
}
std::string Peer::convertIpPortVectorToIpPortString(std::vector<uint8_t> ip_vector, std::vector<uint8_t> port_vector)
{
    std::string ip = buildIp(ip_vector);
    uint16_t port = ntohs(*reinterpret_cast<const uint16_t *>(port_vector.data()));
    return ip + ":" + std::to_string(port);
}
void Peer::sendHandshake()
{
    try
    {
        std::vector<uint8_t> payload(sizeof(alice::ECIPosition) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t));
        std::vector<uint8_t> ip_vector = convertIpToVector(host_ip_);
        std::fill(payload.begin(), payload.end(), 0);
        std::memcpy(payload.data(), ip_vector.data(), sizeof(ip_vector));
        uint16_t network_port = htons(host_port_);
        std::memcpy(payload.data() + 4, &network_port, sizeof(network_port));
        uint8_t peer_type = static_cast<uint8_t>(type_);
        std::memcpy(payload.data() + 6, &peer_type, sizeof(peer_type));
        std::memcpy(payload.data() + 7, &position_.x, sizeof(position_.x));
        std::memcpy(payload.data() + 15, &position_.y, sizeof(position_.y));
        std::memcpy(payload.data() + 23, &position_.z, sizeof(position_.z));
        for (int i = 0; i < payload.size(); i++)
        {
            Logger::log(LogLevel::DEBUG, "Payload in Handshake[" + std::to_string(i) + "]: " + std::to_string(payload[i]));
        }
        alice::Packet registrationPacket = alice::Packet(id_, 12345, alice::PacketType::HANDSHAKE, 255, 0, payload);
        sendData(registrationPacket);
        Logger::log(LogLevel::INFO, "Sent HANDSHAKE packet with position: (" +
                                        std::to_string(position_.x) + ", " +
                                        std::to_string(position_.y) + ", " +
                                        std::to_string(position_.z) + ")");
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Error while sending HANDSHAKE: " + std::string(e.what()));
    }
}
Peer::Peer(const uint32_t &id, PeerType type)
    : id_(id), type_(type), ip_table_(std::make_shared<alice::DeviceIPTable>()), position_table_(std::make_shared<alice::PositionTable>()), position_notifier_(std::make_shared<alice::PositionNotifier>(io_context_, 0, 0, 0, 0)), socket_(io_context_, asio::ip::udp::v4()), encryption_manager_(alice::EncryptionManager()), type_table_(std::make_shared<alice::DeviceTypeTable>())
{
}