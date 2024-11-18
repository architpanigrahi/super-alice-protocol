#include "alice/peer_satellite_node.hpp"

PeerSatelliteNode::PeerSatelliteNode(const uint32_t &id)
    : Peer(id, PeerType::SATELLITE)
{
    orbital_parameters_ = {6871000.0, 0.001, 51.6, 0, 0, 0, 0};
    position_ = alice::ECIPositionCalculator::calculate_position(orbital_parameters_, 1);
    Logger::log(LogLevel::INFO, "Satellite node initialized with ID: " + std::to_string(id) + " and position: (" + std::to_string(position_.x) + ", " + std::to_string(position_.y) + ", " + std::to_string(position_.z) + ")");
}
void PeerSatelliteNode::connect()
{
    Logger::log(LogLevel::INFO, "Connecting satellite node on " + host_ip_ + ":" + std::to_string(host_port_));
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
void PeerSatelliteNode::disconnect()
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
void PeerSatelliteNode::sendData(const alice::Packet &packet)
{
    try
    {
        std::vector<uint8_t> data = packet.serialize(encryption_manager_);
        Logger::log(LogLevel::DEBUG, "Sending data of size: " + std::to_string(data.size()));
        asio::ip::udp::endpoint destination(asio::ip::make_address(bootstrap_ip_), bootstrap_port_);
        socket_.send_to(asio::buffer(data), destination);
        Logger::log(LogLevel::INFO, "Data sent to bootstrap node at " + bootstrap_ip_ + ":" + std::to_string(bootstrap_port_));
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Error while sending data: " + std::string(e.what()));
    }
}
void PeerSatelliteNode::startListening()
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
std::string bytesToHex(const std::vector<uint8_t> &data)
{
    std::ostringstream oss;
    for (uint8_t byte : data)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}
void PeerSatelliteNode::receiveData(const asio::error_code &error, std::size_t bytes_transferred)
{
    if (!error && bytes_transferred > 0)
    {
        std::vector<uint8_t> raw_data(receive_buffer_.begin(), receive_buffer_.begin() + bytes_transferred);
        Logger::log(LogLevel::DEBUG, "Bytes to hex receiving:" + bytesToHex(raw_data));
        try
        {
            alice::Packet packet = alice::Packet::deserialize(raw_data, encryption_manager_);
            Logger::log(LogLevel::DEBUG, "received packet size of " + std::to_string(raw_data.size()));
            switch (packet.type)
            {
            case alice::PacketType::DATA:
            {
                Logger::log(LogLevel::INFO, "Not implemented.");
                break;
            }
            case alice::PacketType::HANDSHAKE:
            {
                Logger::log(LogLevel::INFO, "Not implemented.");
                break;
            }
            case alice::PacketType::CONTROL:
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
            case alice::PacketType::ACK:
            {
                uint32_t id = packet.source_id;
                Logger::log(LogLevel::INFO, "Received control packet from " + std::to_string(id));
                if (packet.payload.size() > 0)
                {
                    if (packet.payload_type == 1)
                    {
                        ip_table_->deserialize(packet.payload);
                        Logger::log(LogLevel::INFO, "IP table updated successfully.");
                        for (const auto &[id, ip] : ip_table_->get_table())
                        {
                            Logger::log(LogLevel::DEBUG, "ID: " + std::to_string(id) + ", IP: " + ip);
                        }
                    }
                }
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
void PeerSatelliteNode::updatePosition()
{
    static std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();

    position_ = alice::ECIPositionCalculator::calculate_position(orbital_parameters_, elapsed_seconds);

    Logger::log(LogLevel::INFO, "Updated position: (" +
                                    std::to_string(position_.x) + ", " +
                                    std::to_string(position_.y) + ", " +
                                    std::to_string(position_.z) + ")");
}
void PeerSatelliteNode::sendKeepAlive()
{
    try
    {
        updatePosition();

        std::vector<uint8_t> payload(sizeof(alice::ECIPosition));
        std::memcpy(payload.data(), &position_, sizeof(position_));

        alice::Packet keepAlivePacket(
            id_,
            12345,
            alice::PacketType::KEEP_ALIVE,
            1,
            0,
            payload);

        sendData(keepAlivePacket);
        Logger::log(LogLevel::INFO, "Sent KEEP_ALIVE packet with position: (" +
                                        std::to_string(position_.x) + ", " +
                                        std::to_string(position_.y) + ", " +
                                        std::to_string(position_.z) + ")");
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Error while sending KEEP_ALIVE: " + std::string(e.what()));
    }
}
int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        Logger::setLogFile("peer_satellite_node.log");
        Logger::log(LogLevel::ERROR, "Usage: ./peer_satellite <id> <host_ip> <host_port> <bootstrap_ip> <bootstrap_port>");
        return 1;
    }

    uint32_t id = std::stoi(argv[1]);
    std::string host_ip = argv[2];
    uint16_t host_port = std::stoi(argv[3]);
    std::string bootstrap_ip = argv[4];
    uint16_t bootstrap_port = std::stoi(argv[5]);

    Logger::setLogFile("peer_satellite_node" + std::to_string(id) + ".log");
    PeerSatelliteNode satelliteNode(id);

    try
    {
        satelliteNode.setHostAddress(host_ip, host_port);
        satelliteNode.setBootstrapAddress(bootstrap_ip, bootstrap_port);
        satelliteNode.connect();
        satelliteNode.startListening();

        alice::ECIPosition position = {0.0, 0.0, 0.0};
        std::vector<uint8_t> payload(sizeof(alice::ECIPosition) + sizeof(std::string));
        std::string ip = host_ip + ":" + std::to_string(host_port);
        std::memcpy(payload.data(), ip.c_str(), ip.size());
        std::memcpy(payload.data() + ip.size(), &position, sizeof(position));

        alice::Packet registrationPacket = alice::Packet(satelliteNode.getID(), 12345, alice::PacketType::HANDSHAKE, 255, 0, payload);
        satelliteNode.sendData(registrationPacket);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        bool running = true;
        while (running)
        {
            satelliteNode.sendKeepAlive();
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }

        satelliteNode.disconnect();
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Exception in main: " + std::string(e.what()));
    }

    return 0;
}
