#ifndef ALTOOLSET_SIN_GENERATOR_HPP
#define ALTOOLSET_SIN_GENERATOR_HPP

#include <iostream>
#include <cmath>
#include <cstring>

#include "altoolset/generator.hpp"

namespace altoolset {

    class SinGenerator : public Generator
    {
    protected:
        ALfloat frequency;
        /**
         * @brief currentAngle tell where we are on sinusoidal circle
         */
        ALfloat currentAngle = 0.0f;

        virtual ALfloat getFrequency() const override;

    public:
        virtual ALdouble generateNextStep() override;

        SinGenerator(ALfloat frequency, ALCint deviceRate):
            Generator(deviceRate), frequency(frequency) {}

        virtual void setOffset(ALfloat offset);

        virtual void generate() override;

        //~SinGenerator() = default;
    };

}

#endif
