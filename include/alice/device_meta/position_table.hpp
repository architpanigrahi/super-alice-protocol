//
// Created by Archit Panigrahi on 11/11/2024.
// Modified by Rokas Paulauskas
//

#ifndef ALICE_POSITION_TABLE_HPP
#define ALICE_POSITION_TABLE_HPP

#include <string>
#include <unordered_map>
#include <chrono>
#include <vector>

namespace alice
{

    struct PositionEntry
    {
        double x{}, y{}, z{};
        std::chrono::time_point<std::chrono::system_clock> timestamp;
    };

    class PositionTable
    {

    public:
        void update_position(uint32_t id, double x, double y, double z);
        uint32_t get_closest_satellite_id(double x, double y, double z) const;
        void remove_outdated_entries(std::chrono::seconds threshold);
        [[nodiscard]] std::vector<uint8_t> serialize() const;
        void deserialize(const std::vector<uint8_t> &data);
        std::unordered_map<uint32_t, PositionEntry> get_table() const;
        ECIPosition get_position(uint32_t id) const;

    private:
        std::unordered_map<uint32_t, PositionEntry> table_;
    };

}

#endif // ALICE_POSITION_TABLE_HPP
