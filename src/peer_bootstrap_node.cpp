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
    // Connection code here
}

void PeerBootstrapNode::disconnect()
{
    Logger::log(LogLevel::INFO, "Disconnecting bootstrap node.");
    // Cleanup code here
}

void PeerBootstrapNode::sendData(const std::vector<uint8_t> &data)
{
    Logger::log(LogLevel::DEBUG, "Sending data of size: " + std::to_string(data.size()));
    // Data sending code here
}

void PeerBootstrapNode::receiveData(std::vector<uint8_t> &data)
{
    Logger::log(LogLevel::DEBUG, "Receiving data...");
    // Data receiving code here
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Logger::log(LogLevel::ERROR, "Usage: ./peer_bootstrap <host_ip> <host_port>");
        return 1;
    }

    std::string host_ip = argv[1];
    uint16_t host_port = std::stoi(argv[2]);
    uint32_t id = 12345;
    PeerBootstrapNode bootstrapNode(id);
    bootstrapNode.setHostAddress(host_ip, host_port);
    bootstrapNode.connect();

    bool running = true;
    while (running)
    {
        std::vector<uint8_t> data;
        bootstrapNode.receiveData(data);

        if (!data.empty())
        {
            Logger::log(LogLevel::INFO, "Received data of size: " + std::to_string(data.size()));
            // Process data here
        }

        // running = false; // Placeholder exit condition
    }

    bootstrapNode.disconnect();
    return 0;
}
