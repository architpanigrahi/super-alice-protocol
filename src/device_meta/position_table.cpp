//
// Created by Archit Panigrahi on 11/11/2024.
//

#include <alice/alice.hpp>
#include <cmath>
#include <limits>

namespace alice {

    void PositionTable::update_position(const uint32_t id, double x, double y, double z) {
        table_[id] = {x, y, z, std::chrono::system_clock::now()};
    }

    uint32_t PositionTable::get_closest_satellite_id(const double x, const double y, const double z) const {
        double min_distance = std::numeric_limits<double>::max();
        uint32_t closest_id{};

        for (const auto& [id, entry] : table_) {
            const double dx = entry.x - x;
            const double dy = entry.y - y;
            const double dz = entry.z - z;
            const double distance = std::sqrt(dx * dx + dy * dy + dz * dz);
            if (distance < min_distance) {
                min_distance = distance;
                closest_id = id;
            }
        }

        return closest_id;
    }

    void PositionTable::remove_outdated_entries(const std::chrono::seconds threshold) {
        auto now = std::chrono::system_clock::now();
        for (auto it = table_.begin(); it != table_.end();) {
            if (std::chrono::duration_cast<std::chrono::seconds>(now - it->second.timestamp) > threshold) {
                it = table_.erase(it);
            } else {
                ++it;
            }
        }
    }

}
