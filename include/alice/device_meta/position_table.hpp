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
    void update_position(const std::string& id, double x, double y, double z);
    std::string get_closest_satellite(double x, double y, double z) const;
    void remove_outdated_entries(std::chrono::seconds threshold);

private:
    std::unordered_map<std::string, PositionEntry> table_;

};


}

#endif //ALICE_POSITION_TABLE_HPP
