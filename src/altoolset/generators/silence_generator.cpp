#include "altoolset/generators/silence_generator.hpp"

namespace altoolset {

void SilenceGenerator::generate()
{
    auto size = this->getDataSize();
    for (ALCint i=0; i<size; i++) {
        this->buffer[i] = 0;
    }
}

}
