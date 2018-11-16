#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#include "altoolset/openal/context.hpp"
#include "altoolset/openal/wav_player.hpp"

namespace altoolset {

WavPlayer::WavPlayer(std::string waveFile):
    waveFile(waveFile)
{}

WavPlayer::~WavPlayer() {
    if (this->initialised) {
        alSourcei(this->source, AL_BUFFER, 0);
        alDeleteSources(1, &this->source);
        alDeleteBuffers(1, &this->buffer);
    }
}

bool WavPlayer::init() {
    alutLoadWAVFile((ALbyte*)this->waveFile.c_str(), &this->format,
            &this->data, &this->size, &this->freq, AL_FALSE);
    std::cerr << "Test wav" << this->data << std::endl;
    //ALCint idat = 0;
    //short* dat = (short*)this->data;
    //for (; idat<this->size && dat[idat]==0; idat++);
    //for (size_t i = 0; i<10; i++) {
    //    std::cerr << "["<<i<<"]:" << dat[idat+i] << " ";
    //}
    //std::cerr << std::endl;
    //std::cerr << "Loaded wav file: " << this->waveFile << " Size:" << this->size << std::endl;

    alGenBuffers((ALuint)1, &this->buffer);
    alGenSources((ALuint)1, &this->source);

    alSourcef (this->source, AL_PITCH, 1);
    alSourcef (this->source, AL_GAIN,  1);
    alSource3f(this->source, AL_POSITION, 0, 0, 0);
    alSource3f(this->source, AL_VELOCITY, 0, 0, 0);
    alSourcei (this->source, AL_LOOPING, this->loop);

    alBufferData(this->buffer, this->format, this->data, this->size, this->freq);

    alSourcei(this->source, AL_BUFFER, this->buffer);
    this->initialised = true;
    return true;
}

void WavPlayer::play() {
    if (!this->initialised) {
        this->init();
    }
    alSourcePlay(this->source);
}

void WavPlayer::stop() {
    if (!this->initialised) {
        return;
    }
    alSourceStop(this->source);
}

void WavPlayer::join() {
    ALint source_state;
    do {
        std::this_thread::sleep_for( std::chrono::milliseconds(10) );
        alGetSourcei(this->source, AL_SOURCE_STATE, &source_state);
    } while(source_state == AL_PLAYING);
}

}
