//
// Created by Archit Panigrahi on 11/11/2024.
//

#ifndef ALICE_ORBITAL_PARAMETERS_HPP
#define ALICE_ORBITAL_PARAMETERS_HPP

#include <string>

namespace alice {

    struct OrbitalParameters {
        double semi_major_axis;
        double eccentricity;
        double inclination;
        double arg_of_perigee;
        double raan;
        double mean_anomaly_at_epoch;
        double epoch_time;
    };

}

#endif //ALICE_ORBITAL_PARAMETERS_HPP
