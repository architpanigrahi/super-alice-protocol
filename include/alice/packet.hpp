//
// Created by Archit Panigrahi on 04/11/2024.
//

#ifndef ALICE_PACKET_HPP
#define ALICE_PACKET_HPP

#include <cstdint>
#include <vector>

namespace alice
{

    enum class PacketType : uint8_t
    {
        HANDSHAKE,
        DATA,
        ACK,
        NACK,
        CONTROL,
        ERROR
    };

    class Packet
    {
    public:
        uint32_t source_id;
        uint32_t destination_id;
        PacketType type;
        uint8_t priority;
        uint32_t sequence_number;
        uint64_t timestamp;
        uint16_t fragment_id;
        uint16_t total_fragments;
        uint16_t crc;
        uint16_t reserved;
        uint16_t payload_type;
        std::vector<uint8_t> payload;

        Packet(uint32_t source_id, uint32_t destination_id, PacketType type, uint8_t priority, uint32_t sequence_number, const std::vector<uint8_t> &payload);

        [[nodiscard]] std::vector<uint8_t> serialize() const;

        static Packet deserialize(const std::vector<uint8_t> &buffer);
    };

}

#endif // ALICE_PACKET_HPP
