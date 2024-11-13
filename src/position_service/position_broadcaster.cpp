//
// Created by Archit Panigrahi on 12/11/2024.
//

#include <alice/alice.hpp>
#include <iostream>

namespace alice {

    PositionBroadcaster::PositionBroadcaster(asio::io_context& io_context, uint32_t source_id, double x, double y, double z)
        : source_id_(source_id), x_(x), y_(y), z_(z), socket_(io_context, asio::ip::udp::v4()) {}


    void PositionBroadcaster::set_position(double x, double y, double z) {
        x_ = x;
        y_ = y;
        z_ = z;
    }

    void PositionBroadcaster::broadcast_position(uint32_t destination_id, const std::string& destination_ip, uint16_t port) {
        uint64_t timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        Packet packet = Packet::create_position_update_packet(source_id_, destination_id, timestamp, x_, y_, z_);

        std::vector<uint8_t> serialized_data = packet.serialize();

        asio::ip::udp::endpoint destination(asio::ip::make_address(destination_ip), port);
        socket_.send_to(asio::buffer(serialized_data), destination);

        std::cout << "Broadcasting position to " << destination_id << " at " << destination_ip << ":" << port << std::endl;
    }



}
