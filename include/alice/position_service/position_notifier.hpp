//
// Created by Archit Panigrahi on 11/11/2024.
//

#ifndef ALICE_POSITION_BROADCASTER_HPP
#define ALICE_POSITION_BROADCASTER_HPP

#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>

namespace alice
{

    class PositionNotifier
    {

    public:
        PositionNotifier(asio::io_context &io_context, uint32_t source_id, double x, double y, double z);
        void broadcast_position(uint32_t destination_id, const std::string &destination_ip, uint16_t port);
        void set_position(double x, double y, double z);

    private:
        uint32_t source_id_;
        double x_, y_, z_;
        asio::ip::udp::socket socket_;
    };

}

#endif // ALICE_POSITION_BROADCASTER_HPP
