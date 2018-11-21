#ifndef ALTOOLSET_SIN_HPP
#define ALTOOLSET_SIN_HPP

#include <vector>
#include <cmath>

namespace altoolset {

void initSinPrecalc();

/**
 * @brief `sin` is an implementation of `std::sin` with
 * precalculated values for angles
 *
 * The difference is small, but in percents, for FloatingSinGenerator
 * with -O2, it gives 14-16ms vs. 20-21ms for `std::sin`, which is ~25% faster.
 */
double sin(double radAngle);
float  sin(float radAngle);

}

#endif
