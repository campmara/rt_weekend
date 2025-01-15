#ifndef RT_WEEKEND_H
#define RT_WEEKEND_H

#include <cmath>
#include <cstdlib>
#include <random>
#include <iostream>
#include <limits>
#include <memory>

// C++ Std Usings

using std::make_shared;
using std::shared_ptr;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double()
{
    // return a random real in [0, 1)
    return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
    // return a random real in [min, max)
    return min + (max - min) * random_double();
}

inline double random_double_2()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

// Common Headers

#include "interval.h"
#include "ray.h"
#include "vec3.h"
#include "color.h"

#endif