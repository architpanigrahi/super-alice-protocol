//
// Created by Archit Panigrahi on 11/11/2024.
//

#ifndef ALICE_ECI_POSITION_HPP
#define ALICE_ECI_POSITION_HPP

#include "orbital_parameters.hpp"

namespace alice {
    struct ECIPosition {
        double x, y, z;
    };

    class ECIPositionCalculator {
    public:
        static ECIPosition calculate_position(const OrbitalParameters &params, double time_since_epoch);
    };
}

#endif //ALICE_ECI_POSITION_HPP
