// Created by Rokas Paulauskas on 14/11/2024, modified later on 19/11/2024 by Ze Yan Liow and Rokas Paulauskas.
#include "alice/peer_edge_node.hpp"

PeerEdgeNode::PeerEdgeNode(const uint32_t &id)
    : Peer(id, PeerType::EDGE_DEVICE)
{
    // latitude_ = -3.1190;   // Replace with your actual latitude
    // longitude_ = -60.0217; // Replace with your actual longitude
    // altitude_ = 92.0;      // Altitude in meters

    // // Calculate position using latitude, longitude, and altitude
    // float R = 6371000.0f + altitude_; // Earth's radius in meters + altitude in meters

    // position_.x = R * std::cos(latitude_ * M_PI / 180.0f) * std::cos(longitude_ * M_PI / 180.0f);
    // position_.y = R * std::cos(latitude_ * M_PI / 180.0f) * std::sin(longitude_ * M_PI / 180.0f);
    // position_.z = R * std::sin(latitude_ * M_PI / 180.0f);
    orbital_parameters_ = {6871000.0, 0.001, 51.6, 0, 0, 0, 0};
    position_ = alice::ECIPositionCalculator::calculate_position(orbital_parameters_, 1);

    Logger::log(LogLevel::INFO, "Edge device initialized with ID: " + std::to_string(id) +
                                    " and position: (" + std::to_string(position_.x) + ", " +
                                    std::to_string(position_.y) + ", " + std::to_string(position_.z) + ")");
}

void PeerEdgeNode::connect()
{
    Logger::log(LogLevel::INFO, "Connecting edge device to bootstrap node at " + bootstrap_ip_ + ":" + std::to_string(bootstrap_port_));

    try
    {
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
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Error while connecting: " + std::string(e.what()));
    }
}

void PeerEdgeNode::disconnect()
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
        Logger::log(LogLevel::INFO, "Edge device disconnected.");
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Error during disconnect: " + std::string(e.what()));
    }
}

void PeerEdgeNode::sendHandshake()
{
    try
    {
        Logger::log(LogLevel::DEBUG, "Preparing handshake with position: (" +
                                         std::to_string(position_.x) + ", " +
                                         std::to_string(position_.y) + ", " +
                                         std::to_string(position_.z) + ")");

        std::vector<uint8_t> payload(sizeof(alice::ECIPosition) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t));

        // Convert host IP to byte vector
        std::vector<uint8_t> ip_vector = convertIpToVector(host_ip_);
        std::fill(payload.begin(), payload.end(), 0);

        // Fill the payload with handshake data
        std::memcpy(payload.data(), ip_vector.data(), sizeof(uint32_t)); // IP
        uint16_t network_port = htons(host_port_);                       // Port (network byte order)
        std::memcpy(payload.data() + 4, &network_port, sizeof(uint16_t));
        uint8_t peer_type = static_cast<uint8_t>(type_); // Peer type
        std::memcpy(payload.data() + 6, &peer_type, sizeof(uint8_t));

        // Serialize position (x, y, z)
        std::memcpy(payload.data() + 7, &position_.x, sizeof(position_.x));  // x
        std::memcpy(payload.data() + 15, &position_.y, sizeof(position_.y)); // y
        std::memcpy(payload.data() + 23, &position_.z, sizeof(position_.z)); // z

        // Log serialized values for debugging
        Logger::log(LogLevel::DEBUG, "Serialized position (x): " + std::to_string(position_.x) +
                                         ", Bytes: " + std::to_string(payload[7]) + " " + std::to_string(payload[8]) + " ...");
        Logger::log(LogLevel::DEBUG, "Serialized position (y): " + std::to_string(position_.y) +
                                         ", Bytes: " + std::to_string(payload[15]) + " " + std::to_string(payload[16]) + " ...");
        Logger::log(LogLevel::DEBUG, "Serialized position (z): " + std::to_string(position_.z) +
                                         ", Bytes: " + std::to_string(payload[23]) + " " + std::to_string(payload[24]) + " ...");

        // Create handshake packet
        alice::Packet handshake_packet(
            id_,                          // Source ID
            12345,                        // Destination ID (bootstrap node ID)
            alice::PacketType::HANDSHAKE, // Packet type
            254,                          // Priority
            0,                            // Sequence number
            payload);

        sendData(handshake_packet);

        Logger::log(LogLevel::INFO, "Handshake packet sent to bootstrap node.");
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Error while sending handshake: " + std::string(e.what()));
    }
}

void PeerEdgeNode::startListening()
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

void PeerEdgeNode::sendData(const alice::Packet &packet)
{
    try
    {
        std::vector<uint8_t> data = packet.serialize(encryption_manager_);
        asio::ip::udp::endpoint destination(asio::ip::make_address(bootstrap_ip_), bootstrap_port_);
        socket_.send_to(asio::buffer(data), destination);
        Logger::log(LogLevel::INFO, "Packet sent to bootstrap node.");
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Error while sending data: " + std::string(e.what()));
    }
}

void PeerEdgeNode::receiveData(const asio::error_code &error, std::size_t bytes_transferred)
{
    if (!error && bytes_transferred > 0)
    {
        if (!error && bytes_transferred > 0)
        {
            std::vector<uint8_t> raw_data(receive_buffer_.begin(), receive_buffer_.begin() + bytes_transferred);

            try
            {
                alice::Packet packet = alice::Packet::deserialize(raw_data, encryption_manager_);
                Logger::log(LogLevel::DEBUG, "Received payload size: " + std::to_string(packet.payload.size()));

                switch (packet.type)
                {
                case alice::PacketType::DISCOVERY:
                    Logger::log(LogLevel::INFO, "Received DISCOVERY_RESPONSE from bootstrap node.");
                    break;
                case alice::PacketType::DATA:
                {
                    Logger::log(LogLevel::DEBUG, "===============================================================================================================");
                    Logger::log(LogLevel::INFO, "Received DATA packet from " + std::to_string(packet.source_id));
                    Logger::log(LogLevel::INFO, "Payload: " + std::string(packet.payload.begin(), packet.payload.end()));
                    break;
                }
                case alice::PacketType::PULL:
                {
                    try
                    {
                        // Extract source IP and port from the received packet's payload
                        std::string source_ip;
                        uint16_t source_port;
                        source_ip = std::to_string(packet.payload[0]) + "." + std::to_string(packet.payload[1]) + "." + std::to_string(packet.payload[2]) + "." + std::to_string(packet.payload[3]);
                        source_port = ntohs(*reinterpret_cast<const uint16_t *>(&packet.payload[4]));
                        std::string source_ip_port = source_ip + ":" + std::to_string(source_port);

                        Logger::log(LogLevel::INFO, "Received PULL request from " + source_ip_port);

                        // Generate data payload as JSON
                        std::ostringstream payload_stream;
                        payload_stream << R"({
                        "sensor_id": "edge001",
                        "location": {
                            "latitude": )"
                                       << latitude_ << R"(,
                            "longitude": )"
                                       << longitude_ << R"(,
                            "altitude_meters": )"
                                       << altitude_ << R"(
                        },
                        "timestamp": ")"
                                       << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << R"(",
                        "data": {
                            "temperature_celsius": )"
                                       << (20.0 + static_cast<float>(rand()) / (RAND_MAX / (40.0 - 20.0))) << R"(,
                            "humidity_percentage": )"
                                       << (30.0 + static_cast<float>(rand()) / (RAND_MAX / (70.0 - 30.0))) << R"(,
                            "co2_ppm": )"
                                       << (350.0 + static_cast<float>(rand()) / (RAND_MAX / (450.0 - 350.0))) << R"(,
                            "smoke_density": {
                                "concentration_micrograms_per_cubic_meter": )"
                                       << (100.0 + static_cast<float>(rand()) / (RAND_MAX / (300.0 - 100.0))) << R"(,
                                "detected": )"
                                       << (rand() % 2 == 0 ? "true" : "false") << R"(
                            }
                        },
                        "metadata": {
                            "battery_level_percentage": )"
                                       << (50.0 + static_cast<float>(rand()) / (RAND_MAX / (100.0 - 50.0))) << R"(,
                            "sensor_health": "operational"
                        }
                    })";

                        std::string payload_json = payload_stream.str();
                        std::vector<uint8_t> payload_data(payload_json.begin(), payload_json.end());

                        alice::Packet response_packet(id_, 123456, alice::PacketType::DATA, 1, 0, payload_data, 0, 1);

                        std::vector<uint8_t> serialized_data = response_packet.serialize(encryption_manager_);

                        std::string ip;
                        std::string port;

                        std::istringstream ip_stream(source_ip_port);
                        if (std::getline(ip_stream, ip, ':') && std::getline(ip_stream, port))
                        {
                            asio::ip::udp::endpoint destination(asio::ip::make_address(ip), std::stoi(port));
                            socket_.send_to(asio::buffer(serialized_data), destination);
                            Logger::log(LogLevel::INFO, "Response sent to " + ip + ":" + port);
                        }
                        else
                        {
                            Logger::log(LogLevel::ERROR, "Error while sending response: Invalid IP address.");
                        }
                    }
                    catch (const std::exception &e)
                    {
                        Logger::log(LogLevel::ERROR, "Error processing PULL request: " + std::string(e.what()));
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
    }
    socket_.async_receive_from(
        asio::buffer(receive_buffer_), remote_endpoint_,
        [this](const asio::error_code &error, std::size_t bytes_transferred)
        {
            receiveData(error, bytes_transferred);
        });
}

#ifndef BUILD_TESTS
int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        Logger::setLogFile("peer_edge_node.log");
        Logger::log(LogLevel::ERROR, "Usage: ./peer_edge <id> <host_ip> <host_port> <bootstrap_ip> <bootstrap_port>");
        return 1;
    }

    uint32_t id = std::stoi(argv[1]);
    std::string host_ip = argv[2];
    uint16_t host_port = std::stoi(argv[3]);
    std::string bootstrap_ip = argv[4];
    uint16_t bootstrap_port = std::stoi(argv[5]);

    Logger::setLogFile("peer_edge_node" + std::to_string(id) + ".log");
    PeerEdgeNode edgeNode(id);

    try
    {
        edgeNode.setHostAddress(host_ip, host_port);
        edgeNode.setBootstrapAddress(bootstrap_ip, bootstrap_port);
        edgeNode.connect();
        edgeNode.startListening();
        edgeNode.sendHandshake();

        bool running = true;
        while (running)
        {
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }

        edgeNode.disconnect();
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Exception in main: " + std::string(e.what()));
    }

    return 0;
}
#endif
