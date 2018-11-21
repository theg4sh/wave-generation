
#include <iostream>
#include <string.h>
#include <thread>
#include <condition_variable>

#include <AL/al.h>
#include <AL/alext.h>

#include "altoolset/generator.hpp"
#include "altoolset/openal/queue_player.hpp"

namespace altoolset {

bool QueuePlayer::init()
{
    alGenSources(1, &this->source);
    alGenBuffers(QueuePlayer::BUFFERS_COUNT, this->buffers);
    // Specific to implementation with short-valued buffer
    const float amplitude = std::numeric_limits<short>::max()-1;
    this->generator.setAmplitude(amplitude);
    return true;
}

QueuePlayer::~QueuePlayer()
{
    alSourcei(this->source, AL_BUFFER, 0);
    alDeleteSources(1, &this->source);
    alDeleteBuffers(QueuePlayer::BUFFERS_COUNT, this->buffers);
}

void QueuePlayer::worker()
{
    // XXX: without filled stack it's not working properly, why?
    for (int i = 0; i<QueuePlayer::BUFFERS_COUNT; i++) {
       this->buffersStack.push(this->buffers[i]);
    }

    alSourcei(this->source, AL_BUFFER, 0);
    alSourcef(this->source, AL_GAIN, 1.0f);
    alSourcei(this->source, AL_SOURCE_RELATIVE, AL_TRUE);
    //alSourcei(this->source, AL_ROLLOFF_FACTOR, 0);

    ALuint buffersHolder[QueuePlayer::BUFFERS_COUNT];
    std::cerr << "Buffers holder contains: ";
    for (uint8_t it = 0; it < QueuePlayer::BUFFERS_COUNT; it++) {
        buffersHolder[it] = this->buffers[it];
        std::cerr << buffersHolder[it] << " ";
    }
    std::cerr << std::endl;

    // Generating
    ALint buffersAvailable = QueuePlayer::BUFFERS_COUNT;
    ALint buffersInQueue = 0;
    for (;this->workerRunning;) {
        alGetSourcei(this->source, AL_BUFFERS_QUEUED, &buffersInQueue);
        if (buffersInQueue-buffersAvailable <= 1) {
            //std::cerr << "Fill - Buffers in queue: " << buffersInQueue-buffersAvailable << " Buffers available: " << buffersAvailable << " => ";
            std::vector<short> data;
            data.resize(this->generator.getDeviceRate());
            for (int it=0; it < buffersAvailable; it++)
            {
                this->generator.generate();

                //const auto size = this->generator.getDataSize();
                this->generator.fillOutputBuffer(data);
                //std::cerr << "[" << data.size() << "] " << data[0] << " ... " << data[data.size()-1] << std::endl;
                alBufferData(buffersHolder[it], AL_FORMAT_MONO16,
                             data.data(), data.size(),
                             this->generator.getDeviceRate());
                //std::cerr << buffersHolder[it] << " ";
            }
            //std::cerr << std::endl;
            //std::this_thread::sleep_for( std::chrono::milliseconds(450) );
            alSourceQueueBuffers(this->source, buffersAvailable, buffersHolder);
            buffersAvailable = 0;

            ALint sourceState = 0;
            alGetSourcei(this->source, AL_SOURCE_STATE, &sourceState);
            if (sourceState != AL_PLAYING) {
                std::cerr << "Set source playing" << std::endl;
                alSourcePlay(this->source);
                std::this_thread::sleep_for( std::chrono::milliseconds(10) );
            }
        }

        alGetSourcei(this->source, AL_BUFFERS_PROCESSED, &buffersAvailable);
        if (buffersAvailable > 0) {
            //std::cerr << "Top - Buffers in queue: " << buffersInQueue << " Buffers available: " << buffersAvailable << std::endl;
            alSourceUnqueueBuffers(this->source, buffersAvailable, buffersHolder);
        }

        std::this_thread::sleep_for( std::chrono::milliseconds(20) );
    }
}


void QueuePlayer::play()
{
    this->workerRunning = true;
    this->workerThread = std::thread(std::bind(&QueuePlayer::worker, this));
}

void QueuePlayer::stop() {
    this->workerRunning = false;
    alSourceStop(this->source);
    this->workerThread.join();
}

/**
 * @brief Wait unless finished
 */
void QueuePlayer::join()
{
    this->workerThread.join();
}

}
