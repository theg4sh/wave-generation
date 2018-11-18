#ifndef ALTOOLSET_GENERATOR_HPP
#define ALTOOLSET_GENERATOR_HPP

#include <iostream>
#include <queue>
#include <vector>
#include <limits>
#include <cmath>
#include <AL/al.h>
#include <AL/alc.h>

#include <altoolset/generator_buffer.hpp>

namespace altoolset {

    const ALdouble CIRCLE = M_PI * 2.0f;

    class Generator
    {
    public:
    private:
        ALCint deviceRate;
    protected:
        /**
         * @brief buffer have size equal to deviceRate value, which means
         * it contains wave for one second.
         */
        GeneratorBuffer buffer;

        ALCint getBufferMaxValue();
        ALCint getBufferMinValue();

        /**
         * @brief feedFrequency uses to change frequency in a tick
         */
        virtual void feedFrequency();

        /**
         * @brief getFrequency uses to finalize frequency value
         */
        virtual ALfloat getFrequency() const = 0;

        bool bufferPush(float value) {
            return this->buffer.push(value);
        }

    public:
        Generator(ALCint deviceRate):
            deviceRate(deviceRate),
            buffer(deviceRate) {}

        ~Generator() = default;

        /**
         * @brief Initialize container
         */
        virtual void init();

        /**
         * @brief generateNextStep
         * TODO: rename that method
         */
        virtual ALdouble generateNextStep() = 0;

        /**
         * @brief return size of data. Expected to be constant.
         */
        std::size_t getDataSize();

        ALCint getDeviceRate() const;

        /**
         * @brief generate should check an getBuffersAvailable and
         * if it -gt 0 then fill next buffer
         */
        virtual void generate(float amplitude) = 0;

        /**
         * @brief fillOutputBuffer will copy full or part of buffer
         * and remember the last position
         */
        template<typename T>
        void fillOutputBuffer(std::vector<T>& dest) {
            std::size_t idst = 0;
            do {
                for (; this->buffer.current() != this->buffer.end()
                       && idst < dest.size(); this->buffer.next(), idst++) {
                    dest[idst] = static_cast<T>(*this->buffer.current());
                }
            } while(idst < dest.size() && this->buffer.current() != this->buffer.end());
        }
        template<typename T>
        void fillOutputBuffer(T* dest, std::size_t destSize) {
            std::size_t idst = 0;
            do {
                for (; this->buffer.current() != this->buffer.end()
                       && idst < destSize; this->buffer.next(), idst++) {
                    dest[idst] = static_cast<T>(*this->buffer.current());
                }
            } while(idst < destSize && this->buffer.current() != this->buffer.end());
        }
    };

}

#endif
