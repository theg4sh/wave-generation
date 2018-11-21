#ifndef VOICE_TONE_GENERATOR_HPP
#define VOICE_TONE_GENERATOR_HPP

#include <memory>
#include "altoolset/generator.hpp"

namespace altoolset {

    class CompositeGenerator : public Generator
    {
    private:
        std::vector<std::pair<std::shared_ptr<Generator>, float>> generators;

        /**
         * @brief currentAngle tell where we are on sinusoidal circle
         */
        ALfloat currentAngle = 0.0f;

        virtual ALfloat getFrequency() const override { return 0.0f; };
    public:
        CompositeGenerator(float deviceRate):
            Generator(deviceRate), generators(0) {}

        void push(std::shared_ptr<Generator> overtone);
        void push(std::shared_ptr<Generator> overtone, float amplitude);
        
        virtual ALdouble generateNextStep() override;

        virtual void generate() override;
    };

}

#endif
