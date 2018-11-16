#ifndef ALTOOLSET_WAV_PLAYER_HPP
#define ALTOOLSET_WAV_PLAYER_HPP

#include <memory>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include "altoolset/player.hpp"

namespace altoolset {

    class WavPlayer: public Player
    {
        std::string waveFile;
        ALsizei size, freq;
        ALenum format;
        ALvoid *data;
        ALboolean loop = AL_FALSE;

        ALuint buffer;
        ALuint source;

        ALenum error = AL_NO_ERROR;
        bool initialised = false;
    public:
        WavPlayer(std::string waveFile);
        ~WavPlayer();

        ALenum getError() const { return this->error; }

        virtual bool init() override;

        virtual void play() override;
        virtual void stop() override;

        /**
         * @brief Wait unless finished
         */
        virtual void join() override;
    };

}

#endif
