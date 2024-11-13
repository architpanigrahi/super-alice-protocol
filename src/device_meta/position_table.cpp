//
// Created by Archit Panigrahi on 11/11/2024.
//

#include <alice/alice.hpp>
#include <cmath>
#include <limits>

namespace alice {

    void PositionTable::update_position(const std::string& id, double x, double y, double z) {
        table_[id] = {x, y, z, std::chrono::system_clock::now()};
    }

    std::string PositionTable::get_closest_satellite(double x, double y, double z) const {
        double min_distance = std::numeric_limits<double>::max();
        std::string closest_id;

        for (const auto& [id, entry] : table_) {
            double dx = entry.x - x, dy = entry.y - y, dz = entry.z - z;
            double distance = std::sqrt(dx * dx + dy * dy + dz * dz);
            if (distance < min_distance) {
                min_distance = distance;
                closest_id = id;
            }
        }

        return closest_id;
    }

    void PositionTable::remove_outdated_entries(std::chrono::seconds threshold) {
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
