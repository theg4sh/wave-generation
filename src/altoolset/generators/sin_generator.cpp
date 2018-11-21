#include <limits>
#include <cassert>

#include "altoolset/sin.hpp"
#include "altoolset/generators/sin_generator.hpp"

namespace altoolset {

ALfloat SinGenerator::getFrequency() const
{
    return this->frequency;
}

void SinGenerator::setOffset(ALfloat offset)
{
    this->currentAngle = offset;
}

ALdouble SinGenerator::generateNextStep()
{
    const ALdouble smpsPerCycle = static_cast<ALdouble>(this->getFrequency()*CIRCLE)/this->getDeviceRate();
    ALdouble newAngle = this->currentAngle + smpsPerCycle;
    this->currentAngle = (newAngle < CIRCLE) ? newAngle : (newAngle - CIRCLE);
    return this->currentAngle;
}

void SinGenerator::generate()
{
    for (ALCint it = 0; it < this->getDeviceRate(); it++) {
        const float value = altoolset::sin(this->currentAngle) * this->amplitude;
        if (!this->bufferPush(value)) {
            return;
        }
        // XXX: why without that set currentAngle does not stored?
        this->currentAngle = this->generateNextStep();
    }
}

}

