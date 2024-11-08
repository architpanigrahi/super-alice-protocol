// test/test_example.cpp

#include <boost/test/unit_test.hpp>
#include <alice/packet.hpp>
#include <vector>

BOOST_AUTO_TEST_CASE(packet_serialize_deserialize)
{
    // Define the packet parameters
    uint32_t source_id = 12345;
    uint32_t destination_id = 54321;
    alice::PacketType type = alice::PacketType::DATA;
    uint8_t priority = 1;
    uint32_t sequence_number = 100;
    std::vector<uint8_t> payload = {10, 20, 30, 40, 50};

    // Create the original packet
    alice::Packet original_packet(source_id, destination_id, type, priority, sequence_number, payload);

    // Serialize the packet
    std::vector<uint8_t> serialized_data = original_packet.serialize();

    // Deserialize the serialized data into a new packet
    alice::Packet deserialized_packet = alice::Packet::deserialize(serialized_data);

    // Verify that the deserialized packet matches the original packet
    BOOST_CHECK_EQUAL(deserialized_packet.source_id, original_packet.source_id);
    BOOST_CHECK_EQUAL(deserialized_packet.destination_id, original_packet.destination_id);
    BOOST_CHECK_EQUAL(static_cast<int>(deserialized_packet.type), static_cast<int>(original_packet.type));
    BOOST_CHECK_EQUAL(deserialized_packet.priority, original_packet.priority);
    BOOST_CHECK_EQUAL(deserialized_packet.sequence_number, original_packet.sequence_number);
    BOOST_CHECK_EQUAL(deserialized_packet.timestamp, original_packet.timestamp);
    BOOST_CHECK(deserialized_packet.payload == original_packet.payload);
}
