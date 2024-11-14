//
// Created by Archit Panigrahi on 04/11/2024.
//

#include <alice/alice.hpp>
#include <cstring>
#include <ctime>

namespace alice
{
    Packet::Packet(uint32_t source_id, uint32_t destination_id, PacketType type, uint8_t priority, uint32_t sequence_number, const std::vector<uint8_t> &payload, uint16_t crc = 0)
        : source_id(source_id), destination_id(destination_id), type(type), priority(priority), sequence_number(sequence_number), timestamp(static_cast<uint64_t>(std::time(nullptr))),
          payload(payload), crc(crc)
    {
    }

    std::vector<uint8_t> Packet::serialize(const EncryptionManager& encryptor) const
    {
        std::vector<uint8_t> buffer;

        buffer.insert(buffer.end(),
                      reinterpret_cast<const uint8_t *>(&source_id),
                      reinterpret_cast<const uint8_t *>(&source_id) + sizeof(source_id));

        buffer.insert(buffer.end(),
                      reinterpret_cast<const uint8_t *>(&destination_id),
                      reinterpret_cast<const uint8_t *>(&destination_id) + sizeof(destination_id));

        buffer.push_back(static_cast<uint8_t>(type));
        buffer.push_back(priority);
        buffer.insert(buffer.end(),
                      reinterpret_cast<const uint8_t *>(&sequence_number),
                      reinterpret_cast<const uint8_t *>(&sequence_number) + sizeof(sequence_number));

        buffer.insert(buffer.end(),
                      reinterpret_cast<const uint8_t *>(&timestamp),
                      reinterpret_cast<const uint8_t *>(&timestamp) + sizeof(timestamp));

        // Perform encryption only for CONTROL & DATA PacketType
        if (type == PacketType::DATA || type == PacketType::CONTROL) {
            std::vector<uint8_t> encrypted_payload = encryptor.encrypt(payload);
            buffer.insert(buffer.end(), encrypted_payload.begin(), encrypted_payload.end());
        } else {
            buffer.insert(buffer.end(), payload.begin(), payload.end());
        }

        const uint16_t crc = crc16(buffer);
        buffer.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>(crc & 0xFF));

        return buffer;
    }

    Packet Packet::deserialize(const std::vector<uint8_t> &buffer, const EncryptionManager& decryptor)
    {
        Packet pkt(0, 0, PacketType::DATA, 0, 0, std::vector<uint8_t>(), 12); // Added the missing sequence_number argument
        size_t offset = 0;

        std::memcpy(&pkt.source_id, &buffer[offset], sizeof(pkt.source_id));
        offset += sizeof(pkt.source_id);

        std::memcpy(&pkt.destination_id, &buffer[offset], sizeof(pkt.destination_id));
        offset += sizeof(pkt.destination_id);

        pkt.type = static_cast<PacketType>(buffer[offset]);
        offset += sizeof(pkt.type);
        pkt.priority = static_cast<uint8_t>(buffer[offset]);
        offset += sizeof(pkt.priority);

        std::memcpy(&pkt.sequence_number, &buffer[offset], sizeof(pkt.sequence_number)); // Deserialize sequence_number
        offset += sizeof(pkt.sequence_number);

        std::memcpy(&pkt.timestamp, &buffer[offset], sizeof(pkt.timestamp));
        offset += sizeof(pkt.timestamp);

        // Extract encrypted payload only for CONTROL & DATA PacketType, without CRC
        if (pkt.type == PacketType::DATA || pkt.type == PacketType::CONTROL) {
            std::vector<uint8_t> encrypted_payload(buffer.begin() + offset, buffer.end() - 2);
            pkt.payload = decryptor.decrypt(encrypted_payload);
        } else {
            pkt.payload.assign(buffer.begin() + offset, buffer.end() - 2);
        }

        uint16_t received_crc = (static_cast<uint16_t>(buffer[buffer.size() - 2]) << 8) | buffer[buffer.size() - 1];
        uint16_t calculated_crc = crc16(std::vector<uint8_t>(buffer.begin(), buffer.end() - 2));

        if (received_crc != calculated_crc) {
            throw std::runtime_error("CRC mismatch");
        }

        pkt.crc = received_crc;

        return pkt;
    }

    uint16_t Packet::crc16(const std::vector<uint8_t> &buffer) {
        uint16_t crc = 0xFFFF;
        for (uint8_t byte : buffer) {
            crc ^= static_cast<uint16_t>(byte) << 8;
            for (int i = 0; i < 8; ++i) {
                if (crc & 0x8000) {
                    crc = (crc << 1) ^ 0x8005;
                } else {
                    crc <<= 1;
                }
            }
        }
        return crc;
    }

}