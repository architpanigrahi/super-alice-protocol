//
// Created by Archit Panigrahi on 11/11/2024.
//

#include <alice/alice.hpp>

namespace alice {

    void SatelliteIPTable::update_ip(const uint32_t id, const std::string& ip_address) {
        ip_table_[id] = ip_address;
    }


    std::string SatelliteIPTable::get_ip(const uint32_t id) const {
        auto it = ip_table_.find(id);
        if (it != ip_table_.end()) {
            return it->second;
        }
        return "";
    }

    void SatelliteIPTable::remove_ip(const uint32_t id) {
        ip_table_.erase(id);
    }


}