#include "geometry.h"

double GetDistance(Point lhs, Point rhs) {
    double pi_per_deg = kPi / 180;

    const double lat1 = lhs.latitude * pi_per_deg;
    const double lat2 = rhs.latitude * pi_per_deg;

    const double long1 = lhs.longitude * pi_per_deg;
    const double long2 = rhs.longitude * pi_per_deg;

    return std::acos(std::sin(lat1) * std::sin(lat2) +
                     std::cos(lat1) * std::cos(lat2) * std::cos(std::abs(long1 - long2))) *
           kRe * 1000.;
}