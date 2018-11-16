#ifndef ALTOOLSET_GENERATOR_HPP
#define ALTOOLSET_GENERATOR_HPP

#include <queue>
#include <vector>
#include <limits>
#include <cmath>
#include <AL/al.h>
#include <AL/alc.h>

namespace altoolset {

    typedef short BufferDataType;
    typedef std::vector<BufferDataType> BufferType;

    const ALdouble CIRCLE = M_PI * 2.0f;

    class Generator
    {
    private:
        ALCint deviceRate;
    protected:
        /**
         * @brief buffer have size equal to deviceRate value, which means
         * it contains wave for one second.
         */
        BufferType buffer;

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

    public:
        Generator(ALCint deviceRate):
            deviceRate(deviceRate)
        {}

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

        const BufferDataType* getData() const;
        /**
         * @brief return size of data. Expected to be constant.
         */
        ALsizei getDataSize() const;

        ALCint getDeviceRate() const;

        virtual void generate() = 0;
    };

}

#endif
