#include "alice/peer_satellite_node.hpp"

PeerSatelliteNode::PeerSatelliteNode(const uint32_t &id)
    : Peer(id, PeerType::SATELLITE)
{
    Logger::log(LogLevel::INFO, "Satellite node initialized with ID: " + std::to_string(id));
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
void PeerSatelliteNode::sendData(const std::vector<uint8_t> &data)
{
    try
    {
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
    Logger::log(LogLevel::INFO, "Satellite node is starting to listen for incoming data.");
    socket_.async_receive_from(
        asio::buffer(receive_buffer_), remote_endpoint_,
        [this](const asio::error_code &error, std::size_t bytes_transferred)
        {
            receiveData(error, bytes_transferred);
            startListening();
        });

    io_thread_ = std::thread([this]
                             { io_context_.run(); });
}
void PeerSatelliteNode::receiveData(const asio::error_code &error, std::size_t bytes_transferred)
{
    if (!error && bytes_transferred > 0)
    {
        std::vector<uint8_t> data(receive_buffer_.begin(), receive_buffer_.begin() + bytes_transferred);
        Logger::log(LogLevel::INFO, "Received response from Bootstrap Node: " +
                                        std::string(data.begin(), data.end()));
    }
    else
    {
        Logger::log(LogLevel::ERROR, "Receive error: " + error.message());
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

        bool running = true;
        while (running)
        {
            // Send dummy data periodically
            std::vector<uint8_t> dummy_data = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"
            satelliteNode.sendData(dummy_data);

            std::this_thread::sleep_for(std::chrono::seconds(2)); // Avoid spamming
        }

        satelliteNode.disconnect();
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Exception in main: " + std::string(e.what()));
    }

    return 0;
}