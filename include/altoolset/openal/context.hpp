#ifndef ALTOOLSET_CONTEXT_HPP
#define ALTOOLSET_CONTEXT_HPP

#include <memory>
#include <vector>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include "altoolset/openal/wav_player.hpp"
#include "altoolset/openal/queue_player.hpp"
#include "altoolset/generator.hpp"

namespace altoolset {

namespace openal {

    class Context
    {
        ALCcontext* context;
        ALCenum error = AL_NO_ERROR;

        std::vector<std::shared_ptr<WavPlayer>>   wavPlayers;
        std::vector<std::shared_ptr<QueuePlayer>> queuePlayers;
    public:
        Context(ALCcontext* context);

        ~Context();

        ALuint getError() const { return this->error; }
        std::string getErrorStr() const { return alutGetErrorString(this->error); }

        ALCcontext* get() const { return this->context; }

        void setCurrent();

        std::shared_ptr<Player> createWavPlayer(std::string waveFile);
        std::shared_ptr<Player> createQueuePlayer(Generator& generator);
    };

} // openal

} // altoolset

#endif
