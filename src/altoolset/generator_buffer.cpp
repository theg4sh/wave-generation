#include "altoolset/generator_buffer.hpp"

namespace altoolset {

GeneratorBuffer::GeneratorBuffer(std::size_t bufferSize)
{
    this->resize(bufferSize);
}

void GeneratorBuffer::resize(std::size_t sz)
{
    // Why sz+1? Bacause here used customized begin and end pointers
    // and with buffer length with the same size as passed, guarantie
    // that pushing element sz-1 will reset buffer size to 0
    this->buffer.resize(sz+1);
    this->_begin = this->buffer.begin();
    this->_end = this->buffer.begin();
}

std::size_t GeneratorBuffer::size()
{
    if (this->begin() <= this->end()) {
        return std::distance(this->begin(), this->end());
    }
    return std::distance(this->begin(), this->buffer.end())
           + std::distance(this->buffer.begin(), this->end());
}

GeneratorBuffer::BufferIter GeneratorBuffer::begin()
{
    return this->_begin;
}

GeneratorBuffer::BufferIter GeneratorBuffer::end()
{
    return this->_end;
}

GeneratorBuffer::BufferIter GeneratorBuffer::current()
{
    return this->_begin;
}

void GeneratorBuffer::next()
{
    if (++this->_begin == this->buffer.end()) {
        this->_begin = this->buffer.begin();
    }
}

bool GeneratorBuffer::push(BufferDataType value)
{
    if (this->_end+1 == this->buffer.end()) {
        if (this->_begin == this->buffer.begin()) {
            return false;
        }
    } else if (this->_end+1 == this->_begin) {
        return false;
    }
    *this->_end = value;
    if (++this->_end == this->buffer.end()) {
        this->_end = this->buffer.begin();
    }
    return true;
}

}
