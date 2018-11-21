#include "altoolset/sin.hpp"
#include "altoolset/generators/composite_generator.hpp"
#include "altoolset/generators/sin_generator.hpp"

namespace altoolset {

void CompositeGenerator::push(std::shared_ptr<Generator> overtone)
{
    this->generators.push_back(std::make_pair(overtone, -1.0f));
}

void CompositeGenerator::push(std::shared_ptr<Generator> overtone, float amplitude)
{
    this->generators.push_back(std::make_pair(overtone, amplitude));
}

ALdouble CompositeGenerator::generateNextStep()
{
    return 0.0f;
}

void CompositeGenerator::generate()
{
    for (ALCint it = 0; it < this->getDeviceRate(); it++) {
        float value = 0.0f;
        for (auto& ga : this->generators) {
            const double amp = ga.second >= 0.0f ? ga.second : this->amplitude;
            // XXX: is there a way to calculate angle and call `sin` once
            //      instead of call `sin` for each generator?
            value += altoolset::sin(ga.first->generateNextStep()) * amp;
        }
        if (!this->bufferPush(value)) {
            return;
        }
    }
}

}
