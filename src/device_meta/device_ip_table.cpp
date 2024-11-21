//
// Created by Archit Panigrahi on 11/11/2024.
// Modified by Rokas Paulauskas
//

#include <alice/alice.hpp>

namespace alice
{

    void DeviceIPTable::update_ip(const uint32_t id, const std::string &ip_address)
    {
        ip_table_[id] = ip_address;
    }
    std::vector<uint8_t> alice::DeviceIPTable::serialize() const
    {
        std::vector<uint8_t> buffer;
        for (const auto &[id, ip_port] : ip_table_)
        {
            uint32_t ip_length = static_cast<uint32_t>(ip_port.size());
            buffer.insert(buffer.end(),
                          reinterpret_cast<const uint8_t *>(&id),
                          reinterpret_cast<const uint8_t *>(&id) + sizeof(id));
            buffer.insert(buffer.end(),
                          reinterpret_cast<const uint8_t *>(&ip_length),
                          reinterpret_cast<const uint8_t *>(&ip_length) + sizeof(ip_length));
            buffer.insert(buffer.end(), ip_port.begin(), ip_port.end());
        }
        return buffer;
    }

    void alice::DeviceIPTable::deserialize(const std::vector<uint8_t> &data)
    {
        size_t i = 0;
        Logger::log(LogLevel::DEBUG, "Data size: " + std::to_string(data.size()));
        while (i < data.size())
        {
            if (i + sizeof(uint32_t) > data.size())
            {
                throw std::runtime_error("Malformed data: Incomplete ID" + std::to_string(i));
            }

            uint32_t id = *reinterpret_cast<const uint32_t *>(&data[i]);
            i += sizeof(uint32_t);

            if (i + sizeof(uint32_t) > data.size())
            {
                throw std::runtime_error("Malformed data: Missing IP length" + std::to_string(i));
            }

            uint32_t ip_length = *reinterpret_cast<const uint32_t *>(&data[i]);
            i += sizeof(uint32_t);

            if (ip_length == 0 || i + ip_length > data.size())
            {
                throw std::runtime_error("Malformed data: IP length exceeds buffer size" + std::to_string(i));
            }

            std::string ip_port(data.begin() + i, data.begin() + i + ip_length);
            i += ip_length;

            size_t colon_pos = ip_port.find(':');
            if (colon_pos == std::string::npos)
            {
                throw std::runtime_error("Malformed data: Invalid IP:Port format" + std::to_string(i));
            }

            ip_table_[id] = ip_port;
        }
    }

    std::unordered_map<uint32_t, std::string> DeviceIPTable::get_table() const
    {
        return ip_table_;
    }
    std::string DeviceIPTable::get_ip(const uint32_t id) const
    {
        auto it = ip_table_.find(id);
        if (it != ip_table_.end())
        {
            return it->second;
        }
        throw std::runtime_error("Device ID has no associated IP");
    }

    void DeviceIPTable::remove_ip(const uint32_t id)
    {
        ip_table_.erase(id);
    }

}