#ifndef ALTOOLSET_QUEUE_PLAYER_HPP
#define ALTOOLSET_QUEUE_PLAYER_HPP

#include <memory>
#include <thread>
#include <atomic>
#include <stack>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include "altoolset/player.hpp"
#include "altoolset/generator.hpp"

namespace altoolset {

    class QueuePlayer: public Player
    {
    private:
        static const uint8_t BUFFERS_COUNT = 4;
        std::stack<ALuint> buffersStack;
        ALuint buffers[QueuePlayer::BUFFERS_COUNT];
        ALuint source;

        std::thread workerThread;
        std::atomic_bool workerRunning;
        void worker();

        const ALCint frequency;
        Generator& generator;
    public:
        QueuePlayer(ALCint frequency, Generator& generator):
            workerRunning(false), frequency(frequency), generator(generator) {}

        ~QueuePlayer();

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
