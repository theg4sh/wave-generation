#ifndef GENERATOR_BUFFER_HPP
#define GENERATOR_BUFFER_HPP

#include <vector>

namespace altoolset {

    /**
     * @brief Simple loop buffer
     * It allow to fill and pop element fully or partially
     */
    class GeneratorBuffer
    {
    public:
        typedef float BufferDataType;
        typedef std::vector<BufferDataType> BufferType;
        typedef std::vector<BufferDataType>::iterator BufferIter;
    protected:
        BufferType buffer;
        BufferIter _begin;
        BufferIter _end;
    public:
        GeneratorBuffer():
            _begin(buffer.end()),
            _end(buffer.end())
        {}

        GeneratorBuffer(std::size_t bufferSize);

        void resize(std::size_t sz);

        std::size_t size();

        BufferIter begin();

        BufferIter end();

        /**
         * @brief Basically, it returns the same value as `begin`, but used in iteration
         */
        BufferIter current();

        /**
         * @brief Switch `current` pointer to the next value in the buffer
         */
        void next();

        /**
         * @brief Push value at the end of looped buffer and returns false if
         * that push call attemt to override the beginning of buffer
         */
        bool push(BufferDataType value);

        friend class Generator;
    };

}

#endif


