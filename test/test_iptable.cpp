// #define BOOST_TEST_MODULE DeviceIPTableTests
#include <boost/test/unit_test.hpp>
#include <alice/alice.hpp>
#include <vector>
#include <string>
#include <unordered_map>

BOOST_AUTO_TEST_SUITE(DeviceIPTableTestSuite)

BOOST_AUTO_TEST_CASE(SerializeAndDeserialize_ValidData)
{
    alice::DeviceIPTable table;
    table.update_ip(1, "192.168.1.1:8080");
    table.update_ip(2, "10.0.0.2:9090");
    table.update_ip(3, "172.16.0.3:7070");

    std::vector<uint8_t> serialized_data = table.serialize();

    alice::DeviceIPTable deserialized_table;
    BOOST_REQUIRE_NO_THROW(deserialized_table.deserialize(serialized_data));

    auto deserialized_map = deserialized_table.get_table();
    BOOST_REQUIRE_EQUAL(deserialized_map.size(), 3);
    BOOST_REQUIRE_EQUAL(deserialized_map[1], "192.168.1.1:8080");
    BOOST_REQUIRE_EQUAL(deserialized_map[2], "10.0.0.2:9090");
    BOOST_REQUIRE_EQUAL(deserialized_map[3], "172.16.0.3:7070");
}

BOOST_AUTO_TEST_CASE(SerializeAndDeserialize_EmptyTable)
{
    alice::DeviceIPTable table;

    std::vector<uint8_t> serialized_data = table.serialize();

    alice::DeviceIPTable deserialized_table;
    BOOST_REQUIRE_NO_THROW(deserialized_table.deserialize(serialized_data));

    BOOST_REQUIRE_EQUAL(deserialized_table.get_table().size(), 0);
}

BOOST_AUTO_TEST_CASE(Deserialize_MalformedData_Truncated)
{
    std::vector<uint8_t> malformed_data = {0x01, 0x00, 0x00};
    alice::DeviceIPTable deserialized_table;

    BOOST_REQUIRE_THROW(deserialized_table.deserialize(malformed_data), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(Deserialize_InvalidIPLength)
{
    std::vector<uint8_t> malformed_data = {
        0x01,
        0x00,
        0x00,
        0x00,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
    };

    alice::DeviceIPTable deserialized_table;
    BOOST_REQUIRE_THROW(deserialized_table.deserialize(malformed_data), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(Deserialize_InvalidIPPortFormat)
{
    alice::DeviceIPTable table;
    table.update_ip(1, "192.168.1.1");

    std::vector<uint8_t> serialized_data = table.serialize();

    alice::DeviceIPTable deserialized_table;
    BOOST_REQUIRE_THROW(deserialized_table.deserialize(serialized_data), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(SerializeAndDeserialize_LongIPPortString)
{
    alice::DeviceIPTable table;
    std::string long_ip_port = std::string(300, 'a') + ":12345";
    table.update_ip(1, long_ip_port);

    std::vector<uint8_t> serialized_data = table.serialize();

    alice::DeviceIPTable deserialized_table;
    BOOST_REQUIRE_NO_THROW(deserialized_table.deserialize(serialized_data));

    auto deserialized_map = deserialized_table.get_table();
    BOOST_REQUIRE_EQUAL(deserialized_map.size(), 1);
    BOOST_REQUIRE_EQUAL(deserialized_map[1], long_ip_port);
}

BOOST_AUTO_TEST_CASE(Deserialize_ValidData_MultipleEntries)
{
    alice::DeviceIPTable table;
    table.update_ip(1, "192.168.1.1:8080");
    table.update_ip(2, "10.0.0.2:9090");

    std::vector<uint8_t> serialized_data = table.serialize();

    alice::DeviceIPTable deserialized_table;
    BOOST_REQUIRE_NO_THROW(deserialized_table.deserialize(serialized_data));

    auto deserialized_map = deserialized_table.get_table();
    BOOST_REQUIRE_EQUAL(deserialized_map.size(), 2);
    BOOST_REQUIRE_EQUAL(deserialized_map[1], "192.168.1.1:8080");
    BOOST_REQUIRE_EQUAL(deserialized_map[2], "10.0.0.2:9090");
}

BOOST_AUTO_TEST_CASE(Deserialize_DuplicateIDs)
{
    alice::DeviceIPTable table;
    table.update_ip(1, "192.168.1.1:8080");
    table.update_ip(1, "10.0.0.1:9090");

    std::vector<uint8_t> serialized_data = table.serialize();

    alice::DeviceIPTable deserialized_table;
    BOOST_REQUIRE_NO_THROW(deserialized_table.deserialize(serialized_data));

    auto deserialized_map = deserialized_table.get_table();
    BOOST_REQUIRE_EQUAL(deserialized_map.size(), 1);
    BOOST_REQUIRE_EQUAL(deserialized_map[1], "10.0.0.1:9090");
}

BOOST_AUTO_TEST_CASE(Deserialize_MixedValidInvalidEntries)
{
    alice::DeviceIPTable table;
    table.update_ip(1, "192.168.1.1:8080");
    table.update_ip(2, "10.0.0.2");

    std::vector<uint8_t> serialized_data = table.serialize();

    alice::DeviceIPTable deserialized_table;
    BOOST_REQUIRE_THROW(deserialized_table.deserialize(serialized_data), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(RemoveIPTest)
{
    alice::DeviceIPTable table;
    table.update_ip(1, "192.168.1.1:8080");
    table.update_ip(2, "10.0.0.2:9090");

    table.remove_ip(1);

    auto table_map = table.get_table();
    BOOST_REQUIRE_EQUAL(table_map.size(), 1);
    BOOST_REQUIRE(table_map.find(1) == table_map.end());
    BOOST_REQUIRE_EQUAL(table_map[2], "10.0.0.2:9090");
}

BOOST_AUTO_TEST_SUITE_END()
