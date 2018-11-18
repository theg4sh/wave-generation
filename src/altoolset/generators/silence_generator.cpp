#include "altoolset/generators/silence_generator.hpp"

namespace altoolset {

void SilenceGenerator::generate(float /*amplitude*/)
{
    for (; this->buffer.current() != this->buffer.end(); this->buffer.next()) {
        if (!this->bufferPush(0)) {
            return;
        }
    }
}

}
