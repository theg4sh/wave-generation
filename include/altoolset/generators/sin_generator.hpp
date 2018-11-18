#ifndef ALTOOLSET_SIN_GENERATOR_HPP
#define ALTOOLSET_SIN_GENERATOR_HPP

#include <iostream>
#include <cmath>
#include <cstring>
#include <vector>

#include "altoolset/generator.hpp"

namespace altoolset {

    class SinGenerator : public Generator
    {
    protected:
        /**
         * @brief sinQuality describes how many substep will be generated in `sinPrecalc`
         * e.g. with sinQuality=1.0f sinPrecalc will contain values for each 1deg angle
         */
        static constexpr const float sinQuality = 1.0f;
        /**
         * @brief sinPrecalc contain results of sin for each angles step
         */
        static std::vector<float> sinPrecalc;
        /**
         * @brief `sin` is an implementation of `std::sin` with
         * precalculated values for angles
         *
         * The difference is small, but in percents, for FloatingSinGenerator
         * with -O2, it gives 14-16ms vs. 20-21ms for `std::sin`, which is ~25% faster.
         */
        static float sin(float angle);

        ALfloat frequency;
        /**
         * @brief currentAngle tell where we are on sinusoidal circle
         */
        ALfloat currentAngle = 0.0f;

        virtual ALfloat getFrequency() const override;

    public:
        virtual void init() override;
        virtual ALdouble generateNextStep() override;

        SinGenerator(ALfloat frequency, ALCint deviceRate):
            Generator(deviceRate), frequency(frequency) {}

        virtual void setOffset(ALfloat offset);

        virtual void generate(float amplitude) override;
    };

}

#endif
