//
// Created by Archit Panigrahi on 11/11/2024.
// Modified By Rokas Paulauskas
//

#include <alice/alice.hpp>
#include <cmath>

namespace alice
{

    ECIPosition ECIPositionCalculator::calculate_position(const OrbitalParameters &params, double time_since_epoch)
    {
        double mu = 3.986004418e14;
        double n = std::sqrt(mu / std::pow(params.semi_major_axis, 3));

        double M = params.mean_anomaly_at_epoch + n * time_since_epoch;

        double E = M;
        for (int i = 0; i < 10; ++i)
        {
            E = M + params.eccentricity * std::sin(E);
        }

        const double nu = 2 * std::atan2(std::sqrt(1 + params.eccentricity) * std::sin(E / 2),
                                         std::sqrt(1 - params.eccentricity) * std::cos(E / 2));
        const double r = params.semi_major_axis * (1 - params.eccentricity * std::cos(E));

        const double rx = r * std::cos(nu);
        const double ry = r * std::sin(nu);

        double cos_O = std::cos(params.raan), sin_O = std::sin(params.raan);
        double cos_w = std::cos(params.arg_of_perigee), sin_w = std::sin(params.arg_of_perigee);
        double cos_i = std::cos(params.inclination), sin_i = std::sin(params.inclination);

        ECIPosition eci{};
        eci.x = (cos_O * cos_w - sin_O * sin_w * cos_i) * rx + (-cos_O * sin_w - sin_O * cos_w * cos_i) * ry;
        eci.y = (sin_O * cos_w + cos_O * sin_w * cos_i) * rx + (-sin_O * sin_w + cos_O * cos_w * cos_i) * ry;
        eci.z = (sin_w * sin_i) * rx + (cos_w * sin_i) * ry;

        return eci;
    }
    double ECIPositionCalculator::distance(const ECIPosition &pos1, const ECIPosition &pos2)
    {
        return std::sqrt(std::pow(pos1.x - pos2.x, 2) + std::pow(pos1.y - pos2.y, 2) + std::pow(pos1.z - pos2.z, 2));
    }

}
