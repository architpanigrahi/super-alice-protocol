#include <boost/test/unit_test.hpp>
#include <alice/peer_satellite_node.hpp>

BOOST_AUTO_TEST_SUITE(PeerDiscoveryTestSuite)

class TestablePeerSatelliteNode : public PeerSatelliteNode
{
public:
    using PeerSatelliteNode::PeerSatelliteNode;

    std::shared_ptr<alice::DeviceIPTable> &getIpTableForTest()
    {
        return ip_table_;
    }
    void setIpTableForTest(std::shared_ptr<alice::DeviceIPTable> ip_table)
    {
        ip_table_ = ip_table;
    }

    alice::EncryptionManager &getEncryptionManager()
    {
        return encryption_manager_;
    }
    void setReceivedDataForTest(const std::vector<uint8_t> &data)
    {
        for (size_t i = 0; i < data.size(); i += 1)
        {
            receive_buffer_[i] = data[i];
        }
    }
};

BOOST_AUTO_TEST_CASE(SendDiscoveryRequest)
{
    alice::Packet discovery_request(1, 12345, alice::PacketType::DISCOVERY, 1, 0, {});

    TestablePeerSatelliteNode satellite_node(1);
    satellite_node.setBootstrapAddress("127.0.0.1", 33001);

    BOOST_REQUIRE_NO_THROW(satellite_node.sendData(discovery_request));

    BOOST_REQUIRE_EQUAL(discovery_request.source_id, 1);
    BOOST_REQUIRE_EQUAL(discovery_request.destination_id, 12345);
    BOOST_REQUIRE_EQUAL(discovery_request.type, alice::PacketType::DISCOVERY);
    BOOST_REQUIRE_EQUAL(discovery_request.payload.size(), 0);
}

BOOST_AUTO_TEST_CASE(ReceiveDiscoveryResponse)
{
    TestablePeerSatelliteNode satellite_node(1);
    satellite_node.setIpTableForTest(std::make_shared<alice::DeviceIPTable>());

    std::string payload = "2:192.168.1.2:33002;3:192.168.1.3:33003;";
    std::vector<uint8_t> serialized_payload(payload.begin(), payload.end());

    alice::Packet discovery_response(12345, 1, alice::PacketType::DISCOVERY, 1, 0, serialized_payload);
    std::vector<uint8_t> serialized_response = discovery_response.serialize(satellite_node.getEncryptionManager());
    satellite_node.setReceivedDataForTest(serialized_response);
    BOOST_REQUIRE_NO_THROW(
        satellite_node.receiveData(asio::error_code(), serialized_response.size()));

    auto ip_table = satellite_node.getIpTableForTest()->get_table();

    BOOST_REQUIRE_EQUAL(ip_table.size(), 2);
    BOOST_REQUIRE_EQUAL(ip_table[2], "192.168.1.2:33002");
    BOOST_REQUIRE_EQUAL(ip_table[3], "192.168.1.3:33003");
}

BOOST_AUTO_TEST_CASE(HandleMalformedDiscoveryResponse)
{
    TestablePeerSatelliteNode satellite_node(1);
    satellite_node.setIpTableForTest(std::make_shared<alice::DeviceIPTable>());

    std::string payload = "2:192.168.1.2:33002;3:192.168.1.3;";
    std::vector<uint8_t> serialized_payload(payload.begin(), payload.end());

    alice::Packet discovery_response(12345, 1, alice::PacketType::DISCOVERY, 1, 0, serialized_payload);
    std::vector<uint8_t> serialized_response = discovery_response.serialize(satellite_node.getEncryptionManager());
    satellite_node.setReceivedDataForTest(serialized_response);
    BOOST_REQUIRE_NO_THROW(
        satellite_node.receiveData(asio::error_code(), serialized_response.size()));

    auto ip_table = satellite_node.getIpTableForTest()->get_table();

    BOOST_REQUIRE_EQUAL(ip_table.size(), 1);
    BOOST_REQUIRE_EQUAL(ip_table[2], "192.168.1.2:33002");
}

// BOOST_AUTO_TEST_CASE(HandleDuplicateDiscoveryResponse)
// {
//     TestablePeerSatelliteNode satellite_node(1);
//     satellite_node.setIpTableForTest(std::make_shared<alice::DeviceIPTable>());

//         alice::Packet discovery_response(12345, 1, alice::PacketType::DISCOVERY, 1, 0, serialized_payload);

//     std::vector<uint8_t> serialized_response = discovery_response.serialize(satellite_node.getEncryptionManager());
//     satellite_node.setReceivedDataForTest(serialized_response);
//     BOOST_REQUIRE_NO_THROW(
//         satellite_node.receiveData(asio::error_code(), serialized_response.size()));

//     auto ip_table = satellite_node.getIpTableForTest()->get_table();

//     BOOST_REQUIRE_EQUAL(ip_table.size(), 1);
//     BOOST_REQUIRE_EQUAL(ip_table[2], "192.168.1.2:33002");
// }

BOOST_AUTO_TEST_CASE(PeriodicDiscovery)
{
    TestablePeerSatelliteNode satellite_node(1);

    BOOST_REQUIRE_NO_THROW(satellite_node.startPeriodicDiscovery());

    std::this_thread::sleep_for(std::chrono::seconds(5));

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
