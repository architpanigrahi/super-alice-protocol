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

void PeerBootstrapNode::sendData(const std::vector<uint8_t> &data)
{
    Logger::log(LogLevel::DEBUG, "Sending data of size: " + std::to_string(data.size()));
    // Data sending code here
}

void PeerBootstrapNode::receiveData(const asio::error_code &error, std::size_t bytes_transferred)
{
    if (!error && bytes_transferred > 0)
    {
        std::vector<uint8_t> data(receive_buffer_.begin(), receive_buffer_.begin() + bytes_transferred);
        Logger::log(LogLevel::INFO, "Received data from " + remote_endpoint_.address().to_string() + ":" +
                                        std::to_string(remote_endpoint_.port()) +
                                        " Size: " + std::to_string(bytes_transferred));

        Logger::log(LogLevel::DEBUG, "Data: " + std::string(data.begin(), data.end()));

        socket_.async_receive_from(
            asio::buffer(receive_buffer_), remote_endpoint_,
            [this](const asio::error_code &error, std::size_t bytes_transferred)
            {
                receiveData(error, bytes_transferred);
            });
    }
    else
    {
        Logger::log(LogLevel::ERROR, "Receive error: " + error.message());
    }
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
