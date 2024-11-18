#include "alice/peer_bootstrap_node.hpp"
#include <string>
#include <vector>

PeerBootstrapNode::PeerBootstrapNode(const uint32_t &id)
    : Peer(id, PeerType::BOOTSTRAP_NODE)
{
    Logger::log(LogLevel::INFO, "Bootstrap node initialized with ID: " + std::to_string(id));
}
void PeerBootstrapNode::connect()
{
    Logger::log(LogLevel::INFO, "Connecting bootstrap node on " + host_ip_ + ":" + std::to_string(host_port_));
    asio::ip::udp::endpoint endpoint(asio::ip::make_address(host_ip_), host_port_);
    socket_ = asio::ip::udp::socket(io_context_, endpoint.protocol());
    if (socket_.is_open())
    {
        Logger::log(LogLevel::DEBUG, "Socket is already open. Closing it.");
        socket_.close();
    }
    socket_.open(endpoint.protocol());
    socket_.bind(endpoint);
}

void PeerBootstrapNode::disconnect()
{
    try
    {
        if (socket_.is_open())
        {
            Logger::log(LogLevel::INFO, "Closing the socket.");
            socket_.close();
        }
        io_context_.stop();
        if (io_thread_.joinable())
        {
            io_thread_.join();
        }
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Error during disconnect: " + std::string(e.what()));
    }
    Logger::log(LogLevel::INFO, "Bootstrap node disconnected.");
}
std::string bytesToHex(const std::vector<uint8_t> &data)
{
    std::ostringstream oss;
    for (uint8_t byte : data)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}
void PeerBootstrapNode::sendData(const alice::Packet &packet)
{
    try
    {
        std::vector<uint8_t> data = packet.serialize(encryption_manager_);
        std::string ip;
        std::string port;
        std::istringstream ip_stream(ip_table_->get_ip(packet.destination_id));
        if (std::getline(ip_stream, ip, ':') && std::getline(ip_stream, port))
        {
            asio::ip::udp::endpoint destination(asio::ip::make_address(ip), std::stoi(port));
            socket_.send_to(asio::buffer(data), destination);
            Logger::log(LogLevel::INFO, "Data sent to device at " + ip + ":" + port);
        }
        else
        {
            Logger::log(LogLevel::ERROR, "Error while sending data: Invalid IP address.");
        }
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Error while sending data: " + std::string(e.what()));
    }
}

void PeerBootstrapNode::receiveData(const asio::error_code &error, std::size_t bytes_transferred)
{
    if (!error && bytes_transferred > 0)
    {
        std::vector<uint8_t> raw_data(receive_buffer_.begin(), receive_buffer_.begin() + bytes_transferred);

        try
        {
            alice::Packet packet = alice::Packet::deserialize(raw_data, encryption_manager_);
            switch (packet.type)
            {
            case alice::PacketType::DATA:
            {
                Logger::log(LogLevel::INFO, "Not implemented.");
                break;
            }
            case alice::PacketType::HANDSHAKE:
            {
                uint32_t id = packet.source_id;
                std::string ip(packet.payload.begin(), packet.payload.end());
                Logger::log(LogLevel::INFO, "Received IP address: " + ip + " for ID: " + std::to_string(id));
                alice::ECIPosition position;
                std::memcpy(&position, packet.payload.data() + ip.size(), sizeof(position));
                Logger::log(LogLevel::INFO, "Registering device: ID=" + std::to_string(id) +
                                                ", IP=" + ip +
                                                ", Position=(" + std::to_string(position.x) + ", " +
                                                std::to_string(position.y) + ", " +
                                                std::to_string(position.z) + ")");
                ip_table_->update_ip(id, ip);
                position_table_->update_position(id, position.x, position.y, position.z);
                Logger::log(LogLevel::INFO, "Device registered successfully.");
                std::vector<uint8_t> payload_ip_table = ip_table_->serialize();
                alice::Packet response_ip_table = alice::Packet(id_, id, alice::PacketType::ACK, 1, 0, payload_ip_table, 0, 1);
                sendData(response_ip_table);
                break;
            }
            case alice::PacketType::KEEP_ALIVE:
            {
                uint32_t id = packet.source_id;
                Logger::log(LogLevel::INFO, "Received KEEP_ALIVE packet from " + std::to_string(id));
                alice::ECIPosition position;
                std::memcpy(&position, packet.payload.data(), sizeof(position));
                Logger::log(LogLevel::INFO, "Received position: (" +
                                                std::to_string(position.x) + ", " +
                                                std::to_string(position.y) + ", " +
                                                std::to_string(position.z) + ")");
                position_table_->update_position(id, position.x, position.y, position.z);
                Logger::log(LogLevel::INFO, "Position updated successfully.");
                break;
            }
            case alice::PacketType::ACK:
            {
                Logger::log(LogLevel::INFO, "Not implemented.");
                break;
            }
            case alice::PacketType::NACK:
            {
                Logger::log(LogLevel::INFO, "Not implemented.");
                break;
            }
            case alice::PacketType::ERROR:
            {
                Logger::log(LogLevel::INFO, "Not implemented.");
                break;
            }
            case alice::PacketType::CONTROL:
            {
                Logger::log(LogLevel::INFO, "Not implemented.");
                break;
            }
            default:
            {
                Logger::log(LogLevel::ERROR, "Unknown packet type received." + std::to_string(static_cast<uint8_t>(packet.type)));
                break;
            }
            }
        }
        catch (const std::exception &e)
        {
            Logger::log(LogLevel::ERROR, "Error while deserializing packet: " + std::string(e.what()));
        }
    }
    else
    {
        Logger::log(LogLevel::ERROR, "Receive error: " + error.message());
    }
    socket_.async_receive_from(
        asio::buffer(receive_buffer_), remote_endpoint_,
        [this](const asio::error_code &error, std::size_t bytes_transferred)
        {
            receiveData(error, bytes_transferred);
        });
}
void PeerBootstrapNode::startListening()
{
    if (listening_)
    {
        Logger::log(LogLevel::ERROR, "Already listening for incoming data.");
        return;
    }
    listening_ = true;
    Logger::log(LogLevel::INFO, "Listening for incoming data on " + host_ip_ + ":" + std::to_string(host_port_));
    socket_.async_receive_from(asio::buffer(receive_buffer_), remote_endpoint_,
                               [this](const asio::error_code &error, std::size_t bytes_transferred)
                               {
                                   receiveData(error, bytes_transferred);
                               });
    io_thread_ = std::thread([this]
                             { io_context_.run(); });
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Logger::setLogFile("peer_bootstrap_node.log");
        Logger::log(LogLevel::ERROR, "Usage: ./peer_bootstrap <host_ip> <host_port>");
        return 1;
    }
    std::string host_ip = argv[1];
    uint16_t host_port = std::stoi(argv[2]);
    uint32_t id = 12345;
    Logger::setLogFile("peer_bootstrap_node.log");
    PeerBootstrapNode bootstrapNode(id);
    try
    {
        bootstrapNode.setHostAddress(host_ip, host_port);
        bootstrapNode.connect();
        bootstrapNode.startListening();

        Logger::log(LogLevel::INFO, "Bootstrap node is running. Press Ctrl+C to stop.");

        std::this_thread::sleep_for(std::chrono::hours(24));
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Exception in main: " + std::string(e.what()));
    }

    bootstrapNode.disconnect();
    return 0;
}
