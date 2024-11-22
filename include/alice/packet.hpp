//
// Created by Archit Panigrahi on 04/11/2024.
// Modified By Rokas Paulauskas(Packet Types, Structure, Encryption only on priority 255), Prakash Narasimhan(crc, using encryption).
//
//

#ifndef ALICE_PACKET_HPP
#define ALICE_PACKET_HPP

#include <cstdint>
#include <vector>

#include "encryption_manager.hpp"

namespace alice
{

    enum class PacketType : uint8_t
    {
        HANDSHAKE,
        DATA,
        DISCOVERY,
        KEEP_ALIVE,
        ROUTE,
        RETRANSMIT,
        PULL
    };
    std::ostream &operator<<(std::ostream &os, const PacketType &type);
    constexpr uint16_t CRC16_POLYNOMIAL = 0x8005;
    constexpr uint16_t CRC16_INITIAL = 0xFFFF;

    class Packet
    {
    public:
        uint32_t source_id;
        uint32_t destination_id;
        PacketType type;
        uint8_t priority;
        uint32_t sequence_number;
        uint64_t timestamp;
        uint16_t fragment_index;
        uint16_t total_fragments;
        uint16_t crc;
        uint16_t reserved;
        uint16_t payload_type;
        std::vector<uint8_t> payload;

        Packet(uint32_t source_id, uint32_t destination_id, PacketType type, uint8_t priority, uint32_t sequence_number,
               const std::vector<uint8_t> &payload, uint16_t crc = 0, uint16_t payload_type = 0,
               uint16_t fragment_index = 0, uint16_t total_fragments = 0);

        [[nodiscard]] std::vector<uint8_t> serialize(const EncryptionManager &encryptor) const;

        [[nodiscard]] std::vector<Packet> fragment(uint16_t maxPayloadSize) const;
        static uint16_t crc16(const std::vector<uint8_t> &buffer);
        static Packet reassemble(const std::vector<Packet> &fragments);
        static Packet deserialize(const std::vector<uint8_t> &buffer, const EncryptionManager &decryptor);
    };

}

#endif // ALICE_PACKET_HPP
