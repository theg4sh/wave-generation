#ifndef ALTOOLSET_SILENCE_GENERATOR_HPP
#define ALTOOLSET_SILENCE_GENERATOR_HPP

#include <iostream>
#include <cmath>
#include <cstring>

#include "altoolset/generator.hpp"

namespace altoolset {

    class SilenceGenerator : public Generator
    {
        ALsizei size, freq;
        ALenum format;
        ALvoid *data;

    public:
        SilenceGenerator(ALCint deviceRate):
            Generator(deviceRate) {}

        virtual void init() override;
        virtual void generate() override;
    };

}

#endif
