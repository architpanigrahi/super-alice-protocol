//
// Created by Archit Panigrahi on 11/11/2024.
//

#ifndef ALICE_POSITION_TABLE_HPP
#define ALICE_POSITION_TABLE_HPP

#include <string>
#include <unordered_map>
#include <chrono>

namespace alice {

struct PositionEntry {
    double x{}, y{}, z{};
    std::chrono::time_point<std::chrono::system_clock> timestamp;
};

class PositionTable {

public:
    void update_position(uint32_t id, double x, double y, double z);
    uint32_t get_closest_satellite(double x, double y, double z) const;
    void remove_outdated_entries(std::chrono::seconds threshold);

private:
    std::unordered_map<uint32_t, PositionEntry> table_;

};


}

#endif //ALICE_POSITION_TABLE_HPP
