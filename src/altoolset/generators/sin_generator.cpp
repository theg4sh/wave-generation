#include <limits>
#include <cassert>

#include "altoolset/generators/sin_generator.hpp"

namespace altoolset {

std::vector<float> SinGenerator::sinPrecalc;

ALfloat SinGenerator::getFrequency() const
{
    return this->frequency;
}

void SinGenerator::setOffset(ALfloat offset)
{
    this->currentAngle = offset;
}

void SinGenerator::init()
{
    Generator::init();

    if (SinGenerator::sinPrecalc.size() == 0) {
        SinGenerator::sinPrecalc.resize(static_cast<std::size_t>(360.0f*this->sinQuality));
        for (std::size_t degAngle=0; degAngle < SinGenerator::sinPrecalc.size(); degAngle++) {
            const float angle = (static_cast<float>(degAngle)/SinGenerator::sinQuality)*M_PI/180.0f;
            SinGenerator::sinPrecalc[degAngle] = std::sin(angle);
        }
    }
}

float SinGenerator::sin(float radAngle) {
    const float degAngle = radAngle*180.0f/M_PI;
    const std::size_t pos = static_cast<std::size_t>(std::floor(degAngle*SinGenerator::sinQuality));
    return SinGenerator::sinPrecalc.at(pos);
}

ALdouble SinGenerator::generateNextStep()
{
    const ALdouble smpsPerCycle = static_cast<ALdouble>(this->getFrequency()*CIRCLE)/this->getDeviceRate();
    ALdouble newAngle = this->currentAngle + smpsPerCycle;
    this->currentAngle = (newAngle < CIRCLE) ? newAngle : (newAngle - CIRCLE);
    return this->currentAngle;
}

void SinGenerator::generate(float amplitude)
{
    for (ALCint it = 0; it < this->getDeviceRate(); it++) {
        const float value = SinGenerator::sin(this->currentAngle) * amplitude;
        if (!this->bufferPush(value)) {
            return;
        }
        this->currentAngle = this->generateNextStep();
    }
}

}

