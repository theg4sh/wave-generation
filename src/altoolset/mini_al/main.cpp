#include <iostream>
#include <stack>
#include <vector>
#include <thread>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#define MINI_AL_IMPLEMENTATION
#include "mini_al.h"

#define DEVICE_FORMAT       mal_format_f32
#define DEVICE_CHANNELS     1
//#define DEVICE_SAMPLE_RATE  48000
#define DEVICE_SAMPLE_RATE  44100

//#include "altoolset/altoolset.hpp"
#include "altoolset/generators/sin_generator.hpp"
#include "altoolset/generators/floating_sin_generator.hpp"

namespace po=boost::program_options;

/*
void WavPlay(std::shared_ptr<altoolset::openal::Context> ctx, size_t count, const char* filename)
{
    std::stack<std::weak_ptr<altoolset::Player>> keeper;
    for (size_t i = 0; i < count; i++) {
        auto wp = ctx->createWavPlayer(filename);
        wp->play();
        keeper.push(wp);
        std::this_thread::sleep_for( std::chrono::milliseconds(900) );
    }
    for (;!keeper.empty();) {
        auto wpweak = keeper.top();
        keeper.pop();
        if (auto wp = wpweak.lock()) {
            wp->join();
        }
        std::cerr << "Done" << std::endl;
    }
}
*/

auto lastTick = std::chrono::high_resolution_clock::now();

// This is the function that's used for sending more data to the device for playback.
// NOTE: avg call each 12ms
mal_uint32 on_send_frames_to_device(mal_device* pDevice, mal_uint32 frameCount, void* pSamples)
{
    mal_assert(pDevice->channels == DEVICE_CHANNELS);

    altoolset::Generator* pSineGen = static_cast<altoolset::Generator*>(pDevice->pUserData);
    mal_assert(pSineGen != nullptr);

    pSineGen->fillOutputBuffer(static_cast<float*>(pSamples), frameCount);
    pSineGen->generate(1.0f);

    return frameCount;
    // see original code in mal_uint64 mal_sine_wave_read_ex
    //return mal_sine_wave_read(pSineWave, frameCount, (float*)pSamples);
}

int QueueSinPlay(ALCint frequency, std::chrono::milliseconds time)
{
    altoolset::SinGenerator generator(frequency, DEVICE_SAMPLE_RATE);
    generator.init();
    generator.generate(1.0f); // prefill which takes -gt 12ms for on_send_frames_to_device

    lastTick = std::chrono::high_resolution_clock::now();
    
    mal_device_config config = mal_device_config_init_playback(DEVICE_FORMAT, DEVICE_CHANNELS, DEVICE_SAMPLE_RATE, on_send_frames_to_device);
    mal_device device;
    if (mal_device_init(NULL, mal_device_type_playback, NULL, &config, &generator, &device) != MAL_SUCCESS) {
        printf("Failed to open playback device.\n");
        return -4;
    }

    printf("Device Name: %s\n", device.name);

    if (mal_device_start(&device) != MAL_SUCCESS) {
        printf("Failed to start playback device.\n");
        mal_device_uninit(&device);
        return -5;
    }
    
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop__em, 0, 1);
#else
    printf("Press Enter to quit...\n");
    getchar();
#endif
    
    mal_device_uninit(&device);
    return 0;
}

int QueueFloatingPlay(ALCint freqMin, ALCint freqMax, ALfloat period, std::chrono::milliseconds time)
{
    altoolset::SinGenerator frequencyWave(period, DEVICE_SAMPLE_RATE);
    frequencyWave.init();
    altoolset::FloatingSinGenerator generator(freqMin, freqMax, &frequencyWave, DEVICE_SAMPLE_RATE);
    generator.generate(1.0f); // prefill which takes -gt 12ms for on_send_frames_to_device

    lastTick = std::chrono::high_resolution_clock::now();
    
    mal_device_config config = mal_device_config_init_playback(DEVICE_FORMAT, DEVICE_CHANNELS, DEVICE_SAMPLE_RATE, on_send_frames_to_device);
    mal_device device;
    if (mal_device_init(NULL, mal_device_type_playback, NULL, &config, &generator, &device) != MAL_SUCCESS) {
        printf("Failed to open playback device.\n");
        return -4;
    }

    printf("Device Name: %s\n", device.name);

    if (mal_device_start(&device) != MAL_SUCCESS) {
        printf("Failed to start playback device.\n");
        mal_device_uninit(&device);
        return -5;
    }
    
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop__em, 0, 1);
#else
    printf("Press Enter to quit...\n");
    getchar();
#endif
    
    mal_device_uninit(&device);
    return 0;
}

int main(int argc, char** argv)
{
    po::options_description desc("General options");
    std::string runType;
    desc.add_options()
        ("help,h", "Print this text")
        ("devices", "List of available devices")
        ("type,t", po::value(&runType), "Command type: wav|file, sin, floatsin");

    std::string filePath;
    po::options_description wavDesc("Play wav file");
    wavDesc.add_options()
        ("file,f", po::value<std::string>(&filePath), "Path to wav file");

    int genFrequency;
    po::options_description sinDesc("Generate sinusoidal");
    sinDesc.add_options()
        ("freq,f", po::value(&genFrequency), "Sound frequency");

    struct _genFloatFrequency {
        int leftFreq;
        int rightFreq;
        float period;
    } genFloatFrequency;
    po::options_description floatSinDesc("Generate floating sinusoidal");
    floatSinDesc.add_options()
        ("leftfreq,l", po::value(&(genFloatFrequency.leftFreq)), "Sound frequency start")
        ("rightfreq,r", po::value(&genFloatFrequency.rightFreq), "Sound frequency end")
        ("period,p", po::value(&genFloatFrequency.period), "Frequency change period (sinusoidal)");

    po::variables_map vm;


    /*
    auto devicesList = altoolset::openal::Device::listAudioDevices();
    if (devicesList.empty()) {
        std::cout << "Cannot found any audio device" << std::endl;
        return 99;
    }

    altoolset::openal::Device device(devicesList[0].c_str());
    if (!device.open()) {
        std::cerr << "main/open: " << device.getError() << std::endl;
        return 1;
    }

    auto ctx = device.createContext();
    if (!ctx) {
        std::cerr << "main/context: " << device.getError() << std::endl;
        return 2;
    }
    ctx->setCurrent();
    */

    try {
        po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);
        if (vm.count("help")) {
            // nop, just fallthrought
        }
        else if (vm.count("devices")) {
            std::cout << "Device list is not supported" << std::endl;
            /*
            std::cout << "Devices list:" << std::endl;
            for (auto device : devicesList) {
                std::cout << "  " << device << std::endl;
            }
            */
            return 0;
        }
        else if (runType == "sin") {
            desc.add(sinDesc);
            po::store(po::parse_command_line(argc, argv, desc), vm);
            po::notify(vm);
            if (!vm.count("freq")) {
                std::cerr << desc << std::endl;
                return 1;
            }
            std::cout << "Sinusoidal generation is not supported yet" << std::endl;
            auto res = QueueSinPlay(genFrequency, std::chrono::seconds(20));
            std::cout << res << std::endl;
            //QueueSinPlay(device, genFrequency, std::chrono::seconds(20), ctx);
            return 0;
        }
        else if (runType == "floatsin") {
            desc.add(floatSinDesc);
            po::store(po::parse_command_line(argc, argv, desc), vm);
            po::notify(vm);
            if (!vm.count("leftfreq") || !vm.count("rightfreq") || !vm.count("period")) {
                std::cerr << desc << std::endl;
                return 1;
            }
            std::cout << "Floating sinusoidal generation is not supported yet" << std::endl;
            auto res = QueueFloatingPlay(genFloatFrequency.leftFreq, genFloatFrequency.rightFreq,
                              (float)genFloatFrequency.period, std::chrono::seconds(60));
            std::cout << res << std::endl;
            //QueueFloatingPlay(device, genFloatFrequency.leftFreq, genFloatFrequency.rightFreq,
            //                  (ALfloat)period, std::chrono::seconds(60), ctx);
            return 0;
        }
        else if (runType == "wav" || runType == "file") {
            /*
            desc.add(wavDesc);
            po::store(po::parse_command_line(argc, argv, desc), vm);
            po::notify(vm);
            if (!vm.count("file")) {
                std::cerr << "File not found: '" << filePath << "'" << std::endl;
                std::cerr << desc << std::endl;
                return 1;
            }
            //filePath = vm["file"].as<std::string>();
            auto fp = boost::filesystem::absolute(boost::filesystem::path(filePath));
            std::cout << fp << std::endl;
            //WavPlay(ctx, 1, fp.c_str());
            */
            std::cout << "Wav player is not supported yet" << std::endl;
            return 0;
        }
    } catch(std::exception &e) {
        // nop
        std::cerr << e.what() << std::endl;
    }
    //desc.add(wavDesc).add(sinDesc).add(floatSinDesc);
    std::cout << desc << std::endl;
    return 0;
}
