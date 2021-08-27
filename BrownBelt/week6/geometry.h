#ifndef MY_PROJECT_GEOMETRY_H
#define MY_PROJECT_GEOMETRY_H

#include <cmath>

const double kPi = 3.1415926535;
const int kRe = 6371;

struct Point {
    double latitude = 0.;
    double longitude = 0.;
};

double GetDistance(Point lhs, Point rhs);

#endif  // MY_PROJECT_GEOMETRY_H
