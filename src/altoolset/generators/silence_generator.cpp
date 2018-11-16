#include "altoolset/generators/silence_generator.hpp"

namespace altoolset {

void SilenceGenerator::init()
{
    auto& buff = this->getBuffer();
    auto size = this->getDataSize();

    buff.clear();
    buff.reserve(size);
}

void SilenceGenerator::generate()
{
    auto& buff = this->getBuffer();
    auto size = this->getDataSize();
    for (ALCint i=0; i<size; i++) {
        buff[i] = 0;
    }
}

}
