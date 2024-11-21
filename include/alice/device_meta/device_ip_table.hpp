//
// Created by Archit Panigrahi on 11/11/2024.
// Modified by Rokas Paulauskas.
//

#ifndef ALICE_DEVICE_IP_TABLE_HPP
#define ALICE_DEVICE_IP_TABLE_HPP

#include <string>
#include <unordered_map>
#include <vector>

namespace alice
{

    class DeviceIPTable
    {

    public:
        void update_ip(uint32_t id, const std::string &ip_address);

        std::string get_ip(uint32_t id) const;
        [[nodiscard]] std::vector<uint8_t> serialize() const;
        void deserialize(const std::vector<uint8_t> &data);
        void remove_ip(uint32_t id);
        std::unordered_map<uint32_t, std::string> get_table() const;

    private:
        std::unordered_map<uint32_t, std::string> ip_table_;
    };

}

#endif // ALICE_DEVICE_IP_TABLE_HPP
