//
// Created by Archit Panigrahi on 11/11/2024.
//

#ifndef ALICE_DEVICE_IP_TABLE_HPP
#define ALICE_DEVICE_IP_TABLE_HPP

#include <string>
#include <unordered_map>

namespace alice {

class SatelliteIPTable {

public:

    void update_ip(uint32_t id, const std::string& ip_address);


    std::string get_ip(uint32_t id) const;


    void remove_ip(uint32_t id);

private:
    std::unordered_map<uint32_t, std::string> ip_table_;

};

}


#endif //ALICE_DEVICE_IP_TABLE_HPP
