//
// Created by Archit Panigrahi on 12/11/2024.
//

#include <alice/alice.hpp>
#include <iostream>

namespace alice
{

    PositionNotifier::PositionNotifier(asio::io_context &io_context, const uint32_t source_id, const double x, const double y, const double z)
        : source_id_(source_id), x_(x), y_(y), z_(z), socket_(io_context, asio::ip::udp::v4()) {}

    void PositionNotifier::set_position(const double x, const double y, const double z)
    {
        x_ = x;
        y_ = y;
        z_ = z;
    }

    void PositionNotifier::broadcast_position(const uint32_t destination_id, const std::string &destination_ip, uint16_t port)
    {
        EncryptionManager encryption_obj;
        uint64_t timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        // Packet packet = Packet::create_position_update_packet(source_id_, destination_id, timestamp, x_, y_, z_);
        Packet packet = Packet(source_id_, destination_id, PacketType::DATA, 1, 0, std::vector<uint8_t>(), 0);
        std::vector<uint8_t> serialized_data = packet.serialize(encryption_obj);

        asio::ip::udp::endpoint destination(asio::ip::make_address(destination_ip), port);
        socket_.send_to(asio::buffer(serialized_data), destination);
    }

}
