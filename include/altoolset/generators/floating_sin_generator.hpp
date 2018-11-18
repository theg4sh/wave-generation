#ifndef ALTOOLSET_FLOATING_SIN_GENERATOR_HPP
#define ALTOOLSET_FLOATING_SIN_GENERATOR_HPP

#include <iostream>
#include <sstream>
#include <cmath>
#include <cstring>
#include <chrono>

//#include "altoolset/generator.hpp"
#include "altoolset/generators/sin_generator.hpp"

namespace altoolset {

    class FloatingSinGenerator : public SinGenerator
    {
    protected:
        ALfloat startFreq, endFreq;

        /**
         * @brief The function describes how we move between startFreq and endFreq
         */
        Generator* frequencyWave;

    public:
        FloatingSinGenerator(ALfloat startFrequency,
                ALfloat endFrequency, 
                Generator* frequencyWave,
                ALdouble deviceRate):
            SinGenerator(startFrequency, deviceRate),
            startFreq(startFrequency),
            endFreq(endFrequency),
            frequencyWave(frequencyWave) {}

    protected: 
        virtual void feedFrequency() override;

    public:
        /**
         * @brief getFrequiency overrides SinGenerator frequency variable
         */
        virtual ALdouble generateNextStep() override;
    };

}

#endif
