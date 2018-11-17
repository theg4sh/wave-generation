#include <iostream>
#include <cassert>
#include "altoolset/generator_buffer.hpp"

class TestGeneratorBuffer : public altoolset::GeneratorBuffer
{
    using GeneratorBuffer = altoolset::GeneratorBuffer;
public:
    TestGeneratorBuffer() :
        GeneratorBuffer() {}

    GeneratorBuffer::BufferIter real_begin()
    {
        return this->buffer.begin();
    }

    GeneratorBuffer::BufferIter real_end()
    {
        return this->buffer.end();
    }

    std::size_t real_size()
    {
        return this->buffer.size();
    }
};


int test_buffer_single_fill()
{
    TestGeneratorBuffer buffer;
    const std::size_t bufsz = 10;
    buffer.resize(bufsz);
    assert(buffer.size() == 0);
    assert(buffer.real_size() == bufsz+1);
    assert(buffer.begin() == buffer.real_begin());
    assert(buffer.end()   == buffer.real_begin());

    std::vector<altoolset::GeneratorBuffer::BufferDataType> check;
    check.resize(bufsz);

    for (std::size_t it = 0; it<bufsz; it++)
    {
        assert(buffer.push(1+it));
        assert(buffer.size() == it+1);
        check[it] = it+1;
    }
    assert(!buffer.push(11));
    assert(buffer.size() == bufsz);

    auto last = buffer.current();
    for(auto ichk = check.begin(); buffer.current() != buffer.end(); buffer.next(), ichk++)
    {
        if (ichk != check.begin()) {
            assert(buffer.current() != last);
        }
        assert(ichk != check.end());
        assert(*buffer.current() == *ichk);
    }
    assert(buffer.size() == 0);
    return 0;
}

int test_buffer_loop_fill()
{
    TestGeneratorBuffer buffer;
    const std::size_t bufsz = 10;
    buffer.resize(bufsz);

    std::vector<altoolset::GeneratorBuffer::BufferDataType> check;
    check.resize(bufsz);

    std::size_t val = 1;
    for (std::size_t rpt = 0; rpt < 100; rpt++) {
        for (std::size_t it = 0; it<bufsz; it++, val++)
        {
            assert(buffer.push(val));
            assert(buffer.size() == it+1);
            check[it] = val;
        }
        assert(!buffer.push(val));
        assert(buffer.size() == bufsz);

        auto last = buffer.current();
        for (auto ichk = check.begin(); buffer.current() != buffer.end(); buffer.next(), ichk++)
        {
            if (ichk != check.begin()) {
                assert(buffer.current() != last);
            }
            //std::cout << *buffer.current() << " " << *ichk << std::endl;
            assert(ichk != check.end());
            assert(*buffer.current() == *ichk);
        }
        assert(buffer.size() == 0);
    }
    return 0;
}

int main(int argc, char** argv)
{
    test_buffer_single_fill();
    test_buffer_loop_fill();
    std::cout << "Passed" << std::endl;
    return 0;
}
