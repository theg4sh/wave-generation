#include <iostream>
#include "altoolset/generator.hpp"

namespace altoolset {

void Generator::feedFrequency() {}

void Generator::init()
{
    this->buffer.resize(this->deviceRate);
}

ALCint Generator::getBufferMaxValue() {
    return std::numeric_limits<short>::max();
}

ALCint Generator::getBufferMinValue() {
    return std::numeric_limits<short>::min();
}

std::size_t Generator::getDataSize() {
    return this->buffer.size();
}

ALCint Generator::getDeviceRate() const {
    return this->deviceRate;
}

}
