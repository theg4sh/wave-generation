#include "altoolset/sin.hpp"

#include <vector>
#include <cmath>

namespace altoolset {

/**
 * @brief sinQuality describes how many substep will be generated in `sinPrecalc`
 * e.g. with sinQuality=1.0f sinPrecalc will contain values for each 1deg angle
 */
const double _sinQuality = 1.0f;

/**
 * @brief sinPrecalc contain results of sin for each angles step
 */
std::vector<double> _sinPrecalc;

void initSinPrecalc()
{
    if (_sinPrecalc.size() == 0) {
        _sinPrecalc.resize(static_cast<std::size_t>(360.0f*_sinQuality));
        for (std::size_t degAngle=0; degAngle < _sinPrecalc.size(); degAngle++) {
            const double angle = (static_cast<double>(degAngle)/_sinQuality)*M_PI/180.0f;
            _sinPrecalc[degAngle] = std::sin(angle);
        }
    }
}

template<typename T>
inline T normalize_angle(T radAngle)
{
    for(; radAngle>2.0*M_PI; radAngle-=2.0f*M_PI);
    for(; radAngle<0.0;      radAngle+=2.0f*M_PI);
    return radAngle;
}

double sin(double radAngle)
{
    const double degAngle = normalize_angle(radAngle)*180.0f/M_PI;
    const std::size_t pos = static_cast<std::size_t>(std::floor(degAngle*_sinQuality));
    return _sinPrecalc.at(pos);
}

float sin(float radAngle)
{
    const float degAngle = normalize_angle(radAngle)*180.0f/M_PI;
    const std::size_t pos = static_cast<std::size_t>(std::floor(degAngle*_sinQuality));
    return _sinPrecalc.at(pos);
}

}
