#include <limits>
#include <cassert>

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
    ALdouble g = 1.0f * (this->getBufferMaxValue()-1);

    const auto size = this->getDataSize();
    assert(size > 0);
    for (ALCint i=0; i<size; i++) {
        this->buffer[i] = static_cast<short>(std::sin(this->currentAngle) * g);
        this->currentAngle = this->generateNextStep();
    }
}

}

