//
// Created by Archit Panigrahi on 11/11/2024.
// Modified By Rokas Paulauskas
//

#include <alice/alice.hpp>
#include <cmath>
#include <limits>

namespace alice
{

    void PositionTable::update_position(const uint32_t id, double x, double y, double z)
    {
        table_[id] = {x, y, z, std::chrono::system_clock::now()};
        Logger::log(LogLevel::INFO, "Position updated for ID: " + std::to_string(id));
        Logger::log(LogLevel::INFO, "Position: (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
    }
    std::vector<uint8_t> PositionTable::serialize() const
    {
        std::vector<uint8_t> buffer;
        for (const auto &[id, entry] : table_)
        {
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&id), reinterpret_cast<const uint8_t *>(&id) + sizeof(id));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&entry.x), reinterpret_cast<const uint8_t *>(&entry.x) + sizeof(entry.x));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&entry.y), reinterpret_cast<const uint8_t *>(&entry.y) + sizeof(entry.y));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&entry.z), reinterpret_cast<const uint8_t *>(&entry.z) + sizeof(entry.z));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t *>(&entry.timestamp), reinterpret_cast<const uint8_t *>(&entry.timestamp) + sizeof(entry.timestamp));
        }
        return buffer;
    }
    ECIPosition PositionTable::get_position(const uint32_t id) const
    {
        if (table_.find(id) != table_.end())
        {
            return {table_.at(id).x, table_.at(id).y, table_.at(id).z};
        }
        else
        {
            throw std::runtime_error("ID not found in position table.");
        }
    }
    void PositionTable::deserialize(const std::vector<uint8_t> &data)
    {
        for (size_t i = 0; i < data.size(); i += sizeof(uint32_t) + 3 * sizeof(double) + sizeof(std::chrono::time_point<std::chrono::system_clock>))
        {
            uint32_t id = *reinterpret_cast<const uint32_t *>(&data[i]);
            double x = *reinterpret_cast<const double *>(&data[i + sizeof(uint32_t)]);
            double y = *reinterpret_cast<const double *>(&data[i + sizeof(uint32_t) + sizeof(double)]);
            double z = *reinterpret_cast<const double *>(&data[i + sizeof(uint32_t) + 2 * sizeof(double)]);
            std::chrono::time_point<std::chrono::system_clock> timestamp = *reinterpret_cast<const std::chrono::time_point<std::chrono::system_clock> *>(&data[i + sizeof(uint32_t) + 3 * sizeof(double)]);
            table_[id] = {x, y, z, timestamp};
        }
    }
    std::unordered_map<uint32_t, PositionEntry> PositionTable::get_table() const
    {
        return table_;
    }
    uint32_t PositionTable::get_closest_satellite_id(const double x, const double y, const double z) const
    {
        double min_distance = std::numeric_limits<double>::max();
        uint32_t closest_id{};

        for (const auto &[id, entry] : table_)
        {
            const double dx = entry.x - x;
            const double dy = entry.y - y;
            const double dz = entry.z - z;
            const double distance = std::sqrt(dx * dx + dy * dy + dz * dz);
            if (distance < min_distance)
            {
                min_distance = distance;
                closest_id = id;
            }
        }

        return closest_id;
    }

    void PositionTable::remove_outdated_entries(const std::chrono::seconds threshold)
    {
        auto now = std::chrono::system_clock::now();
        for (auto it = table_.begin(); it != table_.end();)
        {
            if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second.timestamp) > threshold)
            {
                it = table_.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

}
