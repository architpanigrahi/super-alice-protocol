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

std::vector<uint32_t> PeerSatelliteNode::deserializeRoutePayload(std::vector<uint8_t> &payload) {
    std::vector<uint32_t> routePayload;
    for (size_t i = 0; i < payload.size(); i+=sizeof(uint32_t)) {
        routePayload.push_back(*reinterpret_cast<const uint32_t *>(&payload[i]));
    }
    return routePayload;
}

void PeerSatelliteNode::sendRouteData(const alice::Packet &packet)
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
void PeerSatelliteNode::startPeriodicDiscovery()
{
    std::thread discovery_thread([this]()
                                 {
        while (true) {
            discoverPeers();
            std::this_thread::sleep_for(std::chrono::seconds(30));
        } });
    discovery_thread.detach();
}

void PeerSatelliteNode::getRoutingDetails() {
    alice::Packet discovery_request(id_, 1000000003, alice::PacketType::ROUTE, 1, 0, {});
    sendData(discovery_request);
    Logger::log(LogLevel::INFO, "GET ROUTE request to bootstrap node.");

}

void PeerSatelliteNode::discoverPeers()
{
    try
    {
        alice::Packet discovery_request(id_, 12345, alice::PacketType::DISCOVERY, 1, 0, {});
        sendData(discovery_request);
        Logger::log(LogLevel::INFO, "Sent DISCOVERY request to bootstrap node.");
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Error while sending DISCOVERY request: " + std::string(e.what()));
    }
}
void PeerSatelliteNode::receiveData(const asio::error_code &error, std::size_t bytes_transferred)
{
    if (!error && bytes_transferred > 0)
    {
        std::vector<uint8_t> raw_data(receive_buffer_.begin(), receive_buffer_.begin() + bytes_transferred);
        try
        {
            alice::Packet packet = alice::Packet::deserialize(raw_data, encryption_manager_);
            Logger::log(LogLevel::DEBUG, "received packet size of " + std::to_string(raw_data.size()));
            switch (packet.type)
            {
            case alice::PacketType::DATA:
            {
                Logger::log(LogLevel::INFO, "PACKET INFO"+ std::to_string(packet.source_id) + ":"+ std::to_string(packet.destination_id));
                for (uint32_t val : packet.payload) {
                    // Cast to int to avoid interpreting as char
                    Logger::log(LogLevel::INFO, "ROUTED DATA : " + std::to_string(val));
                }
                break;
            }
            case alice::PacketType::HANDSHAKE:
            {
                Logger::log(LogLevel::INFO, "Not implemented.");
                break;
            }
            case alice::PacketType::ROUTE: {
                Logger::log(LogLevel::INFO, "Received Route Info from Bootstrap");

                // Deserialize the route payload into a vector of satellite IDs
                std::vector<uint32_t> routePayload = deserializeRoutePayload(packet.payload);

                if (routePayload.empty()) {
                    Logger::log(LogLevel::ERROR, "Route payload is empty. Cannot proceed with routing.");
                    break;
                }

                Logger::log(LogLevel::DEBUG, "Route payload contains " + std::to_string(routePayload.size()) + " entries.");
                for (uint32_t satelliteID : routePayload) {
                    Logger::log(LogLevel::DEBUG, "Satellite ID in route: " + std::to_string(satelliteID));
                }

                if (routePayload.size() == 1) {
                    // If there is only one satellite in the route, send a DATA packet to it
                    uint32_t targetSatelliteID = routePayload[0];

                    Logger::log(LogLevel::INFO, "Only one satellite in route. Sending DATA packet to Satellite ID=" + std::to_string(targetSatelliteID));

                    // Construct the payload {1, 2, 3}
                    std::vector<uint8_t> dataPayload = {1, 2, 3};

                    // Create a DATA packet
                    alice::Packet dataPacket(
                        id_,
                        targetSatelliteID,
                        alice::PacketType::DATA,
                        1, // version
                        0, // flags
                        dataPayload
                    );

                    // Send the DATA packet
                    sendRouteData(dataPacket);

                    Logger::log(LogLevel::INFO, "Sent DATA packet to Satellite ID=" + std::to_string(targetSatelliteID));
                } else {
                    // Intermediate hop - forward the packet to the next satellite in the route
                    uint32_t nextSatelliteID = routePayload[0]; // Assuming the first ID in the list is the next hop
                    routePayload.erase(routePayload.begin()); // Remove the current satellite from the route

                    // Serialize the updated route payload
                    std::vector<uint8_t> updatedPayload(routePayload.size() * sizeof(uint32_t));
                    std::memcpy(updatedPayload.data(), routePayload.data(), updatedPayload.size());

                    // Create a new packet with the updated route
                    alice::Packet forwardPacket(
                        id_,
                        nextSatelliteID,
                        alice::PacketType::ROUTE,
                        1,
                        0,
                        updatedPayload
                    );

                    // Send the packet to the next satellite
                    sendRouteData(forwardPacket);
                    Logger::log(LogLevel::INFO, "Forwarded route packet to Satellite ID=" + std::to_string(nextSatelliteID));
                }
                break;
            }
            case alice::PacketType::DISCOVERY:
            {
                Logger::log(LogLevel::INFO, "Received DISCOVERY_RESPONSE from bootstrap node.");
                for (int i = 0; i < packet.payload.size(); i++)
                {
                    Logger::log(LogLevel::INFO, "Received DISCOVERY_RESPONSE from bootstrap node." + std::to_string(packet.payload[i]));
                }
                for (int i = 0; i < packet.payload.size(); i += 11)
                {
                    uint32_t peer_id = *reinterpret_cast<const uint32_t *>(&packet.payload[i]);
                    uint8_t type = packet.payload[i + 4];
                    PeerType peer_type = static_cast<PeerType>(type);
                    std::string ip = std::to_string(packet.payload[i + 5]) + "." + std::to_string(packet.payload[i + 6]) + "." + std::to_string(packet.payload[i + 7]) + "." + std::to_string(packet.payload[i + 8]);
                    uint16_t port = ntohs(*reinterpret_cast<const uint16_t *>(&packet.payload[i + 9]));
                    std::string ip_port = ip + ":" + std::to_string(port);
                    ip_table_->update_ip(peer_id, ip_port);
                    type_table_->update_type(peer_id, peer_type);
                    Logger::log(LogLevel::INFO, "Discovered peer: ID=" + std::to_string(peer_id) + "peer type: " + std::to_string(type) + ", IP=" + ip + ", Port=" + std::to_string(port));
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
#ifndef BUILD_TESTS
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
        satelliteNode.sendHandshake();
        satelliteNode.startPeriodicDiscovery();

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
#endif
