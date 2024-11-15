#include "alice/peer_edge_node.hpp"

PeerEdgeNode::PeerEdgeNode(const uint32_t &id)
    : Peer(id, PeerType::EDGE_DEVICE)
{
    Logger::log(LogLevel::INFO, "Edge device initialized with ID: " + std::to_string(id));
}
void PeerEdgeNode::startListening()
{
    // pass
}
void PeerEdgeNode::receiveData(const asio::error_code &error, std::size_t bytes_transferred)
{
    Logger::log(LogLevel::DEBUG, "Receiving data...");
    // Data receiving code here
}
void PeerEdgeNode::sendData(const std::vector<uint8_t> &data)
{
    Logger::log(LogLevel::DEBUG, "Sending data of size: " + std::to_string(data.size()));
    // Data sending code here
}
void PeerEdgeNode::connect()
{
    Logger::log(LogLevel::INFO, "Connecting edge device on " + host_ip_ + ":" + std::to_string(host_port_));
    asio::ip::udp::endpoint endpoint(asio::ip::make_address(host_ip_), host_port_);
    asio::ip::udp::socket socket(io_context_, endpoint.protocol());
    socket.connect(endpoint);
    io_context_.run();
}
void PeerEdgeNode::disconnect()
{
    Logger::log(LogLevel::INFO, "Disconnecting edge device.");
    // Cleanup code here
}
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
    edgeNode.setHostAddress(host_ip, host_port);
    edgeNode.setBootstrapAddress(bootstrap_ip, bootstrap_port);
    edgeNode.connect();
    bool running = true;
    while (running)
    {
        std::vector<uint8_t> data;
        // edgeNode.receiveData(data);
        if (!data.empty())
        {
            Logger::log(LogLevel::INFO, "Received data of size: " + std::to_string(data.size()));
            // Process data here
        }
    }
    return 0;
}