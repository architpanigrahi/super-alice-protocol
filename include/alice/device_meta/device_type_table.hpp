// Created by Rokas Paulauskas on 19/11/2024

#ifndef DEVICE_TYPE_TABLE_HPP
#define DEVICE_TYPE_TABLE_HPP
#include <unordered_map>
#include "peer_enums.hpp"

namespace alice
{
    class DeviceTypeTable
    {
    public:
        void update_type(const uint32_t id, PeerType type);
        PeerType get_type(const uint32_t id) const;
        void remove_type(const uint32_t id);
        std::unordered_map<uint32_t, PeerType> get_table() const;

    private:
        std::unordered_map<uint32_t, PeerType> type_table_;
    };
}
#endif