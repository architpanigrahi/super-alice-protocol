//
// Created by Archit Panigrahi on 04/11/2024.
//

#include <alice/alice.hpp>
#include <cstring>
#include <ctime>
#include <alice/logger.hpp>

namespace alice
{
    Packet::Packet(uint32_t source_id, uint32_t destination_id, PacketType type, uint8_t priority, uint32_t sequence_number,
    const std::vector<uint8_t> &payload, uint16_t crc, uint16_t fragment_id, uint16_t fragment_index, uint16_t total_fragments)
        : source_id(source_id), destination_id(destination_id), type(type), priority(priority), sequence_number(sequence_number),
            timestamp(static_cast<uint64_t>(std::time(nullptr))),
            payload(payload), crc(crc), fragment_id(fragment_id), fragment_index(fragment_index), total_fragments(total_fragments)
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
              reinterpret_cast<const uint8_t *>(&fragment_id),
              reinterpret_cast<const uint8_t *>(&fragment_id) + sizeof(fragment_id));

        buffer.insert(buffer.end(),
      reinterpret_cast<const uint8_t *>(&fragment_index),
      reinterpret_cast<const uint8_t *>(&fragment_index) + sizeof(fragment_index));

        buffer.insert(buffer.end(),
              reinterpret_cast<const uint8_t *>(&total_fragments),
              reinterpret_cast<const uint8_t *>(&total_fragments) + sizeof(total_fragments));

        buffer.insert(buffer.end(),
                      reinterpret_cast<const uint8_t *>(&timestamp),
                      reinterpret_cast<const uint8_t *>(&timestamp) + sizeof(timestamp));

        buffer.insert(buffer.end(), payload.begin(), payload.end());

        const uint16_t crc = crc16(buffer);
        buffer.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>(crc & 0xFF));

        return buffer;
    }

    Packet Packet::deserialize(const std::vector<uint8_t> &buffer)
    {
        Packet pkt(0, 0, PacketType::DATA, 0, 0, std::vector<uint8_t>(), 0); // Added the missing sequence_number argument
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

        std::memcpy(&pkt.fragment_id, &buffer[offset], sizeof(pkt.fragment_id)); // Deserialize fragment_id
        offset += sizeof(pkt.fragment_id);

        std::memcpy(&pkt.fragment_index, &buffer[offset], sizeof(pkt.fragment_index)); // Deserialize fragment_index
        offset += sizeof(pkt.fragment_index);

        std::memcpy(&pkt.total_fragments, &buffer[offset], sizeof(pkt.total_fragments)); // Deserialize total_fragments
        offset += sizeof(pkt.total_fragments);

        std::memcpy(&pkt.timestamp, &buffer[offset], sizeof(pkt.timestamp));
        offset += sizeof(pkt.timestamp);

        pkt.payload.assign(buffer.begin() + offset, buffer.end() - 2);

        uint16_t received_crc = (static_cast<uint16_t>(buffer[buffer.size() - 2]) << 8) | buffer[buffer.size() - 1];
        uint16_t calculated_crc = crc16(std::vector<uint8_t>(buffer.begin(), buffer.end() - 2));
        if (received_crc != calculated_crc) {
            throw std::runtime_error("CRC mismatch");
        }

        pkt.crc = received_crc;

        return pkt;
    }

    std::vector<Packet> Packet::fragment(uint16_t maxPayloadSize) const {
        std::vector<Packet> fragments;
        size_t payloadSize = payload.size();
        uint8_t total = (payloadSize + maxPayloadSize - 1) / maxPayloadSize; // Total number of fragments
        for (uint8_t i = 0; i < total; ++i) {
            size_t start = i * maxPayloadSize;
            size_t end = std::min(start + maxPayloadSize, payloadSize);

            std::vector<uint8_t> fragmentPayload(payload.begin() + start, payload.begin() + end);

            Packet fragment(source_id, destination_id, type, priority, sequence_number, fragmentPayload, crc,
            fragment_id, i, total);
            fragments.push_back(fragment);
        }
        return fragments;
    }

    Packet Packet::reassemble(const std::vector<Packet>& fragments) {
        if (fragments.empty()) {
            throw std::invalid_argument("No fragments to reassemble.");
        }

        std::vector<Packet> sortedFragments = fragments;
        std::sort(sortedFragments.begin(), sortedFragments.end(),
                  [](const Packet& a, const Packet& b) { return a.fragment_id < b.fragment_id; });

        uint32_t expected_id = sortedFragments[0].fragment_id;
        uint8_t expected_total = sortedFragments[0].total_fragments;
        std::vector<uint8_t> reassembledPayload;

        for (const auto& fragment : sortedFragments) {
            if (fragment.fragment_id != expected_id || fragment.total_fragments != expected_total) {
                Logger::log(LogLevel::ERROR, "Fragments mismatch: unable to reassemble.");
            }
            reassembledPayload.insert(reassembledPayload.end(), fragment.payload.begin(), fragment.payload.end());
        }

        Packet reassembledPacket(
            sortedFragments[0].source_id, sortedFragments[0].destination_id,
            sortedFragments[0].type, sortedFragments[0].priority, sortedFragments[0].sequence_number,
            reassembledPayload, 0, sortedFragments[0].fragment_id, 0, 0);

        std::vector<uint8_t> serialized_data = reassembledPacket.serialize();
        uint16_t reassemled_packet_crc = (static_cast<uint16_t>(serialized_data[serialized_data.size() - 2]) << 8) | serialized_data[serialized_data.size() - 1];

        reassembledPacket.crc = reassemled_packet_crc;

        return reassembledPacket;
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