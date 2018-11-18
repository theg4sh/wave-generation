#include <iostream>
#include <limits>

#include "altoolset/generators/floating_sin_generator.hpp"

namespace altoolset {

void FloatingSinGenerator::feedFrequency()
{
    const ALdouble frequencyPosition = this->frequencyWave->generateNextStep();
    const ALfloat range = this->endFreq - this->startFreq;
    //                                               ([-1.0f..1.0f]   ->  [0.0f..2.0f])
    this->frequency = this->startFreq + (range/2.0f)*(SinGenerator::sin(frequencyPosition)+1.0f);
}

ALdouble FloatingSinGenerator::generateNextStep()
{
    this->feedFrequency();
    return this->SinGenerator::generateNextStep();
}

}

