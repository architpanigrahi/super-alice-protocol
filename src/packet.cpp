//
// Created by Archit Panigrahi on 04/11/2024.
//

#include <alice/alice.hpp>
#include <cstring>
#include <ctime>

namespace alice
{
    Packet::Packet(uint32_t source_id, uint32_t destination_id, PacketType type, uint8_t priority, uint32_t sequence_number, const std::vector<uint8_t> &payload)
        : source_id(source_id), destination_id(destination_id), type(type), priority(priority), sequence_number(sequence_number), timestamp(static_cast<uint64_t>(std::time(nullptr))),
          payload(payload)
    {
    }

    std::vector<uint8_t> Packet::serialize() const
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

        buffer.insert(buffer.end(), payload.begin(), payload.end());

        return buffer;
    }

    Packet Packet::deserialize(const std::vector<uint8_t> &buffer)
    {
        Packet pkt(0, 0, PacketType::DATA, 0, std::vector<uint8_t>());
        size_t offset = 0;

        std::memcpy(&pkt.source_id, &buffer[offset], sizeof(pkt.source_id));
        offset += sizeof(pkt.source_id);

        std::memcpy(&pkt.destination_id, &buffer[offset], sizeof(pkt.destination_id));
        offset += sizeof(pkt.destination_id);

        pkt.type = static_cast<PacketType>(buffer[offset]);
        offset += sizeof(pkt.type);
        pkt.priority = static_cast<uint8_t>(buffer[offset]);
        offset += sizeof(pkt.priority);
        std::memcpy(&pkt.sequence_number, &buffer[offset], sizeof(pkt.sequence_number));
        offset += sizeof(pkt.sequence_number);

        std::memcpy(&pkt.timestamp, &buffer[offset], sizeof(pkt.timestamp));
        offset += sizeof(pkt.timestamp);

        pkt.payload.assign(buffer.begin() + offset, buffer.end());

        return pkt;
    }

}