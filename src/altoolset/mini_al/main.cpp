#include <iostream>
#include <stack>
#include <vector>
#include <thread>
#include <unordered_map>

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
#include "altoolset/sin.hpp"
#include "altoolset/generator.hpp"
#include "altoolset/generators/sin_generator.hpp"
#include "altoolset/generators/floating_sin_generator.hpp"
#include "altoolset/generators/composite_generator.hpp"

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

mal_uint32 on_send_frames_to_device(mal_device* pDevice, mal_uint32 frameCount, void* pSamples)
{
    mal_assert(pDevice->channels == DEVICE_CHANNELS);

    altoolset::Generator* pGen = static_cast<altoolset::Generator*>(pDevice->pUserData);
    mal_assert(pGen != nullptr);

    pGen->fillOutputBuffer(static_cast<float*>(pSamples), frameCount);
    pGen->generate();

    // see original code in mal_uint64 mal_sine_wave_read_ex
    //return mal_sine_wave_read(pSineWave, frameCount, (float*)pSamples);
    return frameCount;
}

// This is the function that's used for sending more data to the device for playback.
// NOTE: avg call each 12ms

int play_generator(altoolset::Generator* generator)
{
    assert(generator != nullptr);
    lastTick = std::chrono::high_resolution_clock::now();
    
    // XXX: pass amplitude into on_send_frames_to_devices
    mal_device_config config = mal_device_config_init_playback(
                    DEVICE_FORMAT, DEVICE_CHANNELS, DEVICE_SAMPLE_RATE,
                    on_send_frames_to_device);
    mal_device device;
    if (mal_device_init(NULL, mal_device_type_playback, NULL, &config, generator, &device) != MAL_SUCCESS) {
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

int QueueSinPlay(ALCint frequency, std::chrono::milliseconds time)
{
    altoolset::SinGenerator generator(frequency, DEVICE_SAMPLE_RATE);
    generator.init();
    generator.setAmplitude(1.0f);
    generator.generate(); // prefill which takes -gt 12ms for on_send_frames_to_device

    return play_generator(&generator);
}

int QueueFloatingPlay(ALCint freqMin, ALCint freqMax, ALfloat period, std::chrono::milliseconds time)
{
    auto frequencyWave = std::make_shared<altoolset::SinGenerator>(period, DEVICE_SAMPLE_RATE);
    //frequencyWave.init();
    altoolset::FloatingSinGenerator generator(freqMin, freqMax, frequencyWave, DEVICE_SAMPLE_RATE);
    generator.setAmplitude(1.0f);
    generator.generate(); // prefill which takes -gt 12ms for on_send_frames_to_device

    return play_generator(&generator);
}

std::shared_ptr<altoolset::CompositeGenerator> makeLetterCompositeGenerator(float baseFrequency, float baseAmplitude,
                                                           std::vector<std::pair<float /*frequency*/, float /*amplitude*/>> overtones)
{
    auto frequencyWave = std::make_shared<altoolset::SinGenerator>(0.25f, DEVICE_SAMPLE_RATE);
    auto baseGen = std::make_shared<altoolset::FloatingSinGenerator>(baseFrequency, baseFrequency+25.0f, frequencyWave, DEVICE_SAMPLE_RATE);
    auto generator = std::make_shared<altoolset::CompositeGenerator>(DEVICE_SAMPLE_RATE);
    generator->init();
    generator->push(baseGen, baseAmplitude);
    for (auto& freqAmp : overtones) {
        auto formanta = std::make_shared<altoolset::SinGenerator>(freqAmp.first, DEVICE_SAMPLE_RATE);
        generator->push(formanta, freqAmp.second);
    }
    float amp = 1.0f/(1.0f + static_cast<float>(overtones.size()));
    generator->setAmplitude(amp);
    generator->generate(); // prefill which takes -gt 12ms for on_send_frames_to_device
    return generator;
}
int QueueCompositePlay(ALCint /*frequency*/, std::chrono::milliseconds /*time*/)
{
    /**
                Частота речевых формант
      Гласные   1-я форманта 2-я форманта 3-я форманта

            У   300 625 2500
            О   535 780 2500
            А   700 1080 2600
            Е   440 1800 2550
            И   240 2250 3200
            Ы   300 1480 2230
    */

    std::unordered_map<std::string, std::shared_ptr<altoolset::CompositeGenerator>> lettersMap;
    //      У   300  625  2500
    //altoolset::SinGenerator frm1(300*k, DEVICE_SAMPLE_RATE), frm2(625*k, DEVICE_SAMPLE_RATE), frm3(2500*k, DEVICE_SAMPLE_RATE);
    lettersMap.emplace("У", makeLetterCompositeGenerator(375.0f, 0.650f, {{300.0f, 0.250f}, {625.0f, 0.025f}, {2500.0f, 0.003f}}));
    //      О   535  780  2500
    //altoolset::SinGenerator frm1(535*k, DEVICE_SAMPLE_RATE), frm2(780*k, DEVICE_SAMPLE_RATE), frm3(2500*k, DEVICE_SAMPLE_RATE);
    //      А   700  1080  2600
    //altoolset::SinGenerator frm1(700.0*k, DEVICE_SAMPLE_RATE), frm2(1080.0f*k, DEVICE_SAMPLE_RATE), frm3(2600.0f*k, DEVICE_SAMPLE_RATE);
    //      Е   440  1800  2550
    //altoolset::SinGenerator frm1(440*k, DEVICE_SAMPLE_RATE), frm2(1800*k, DEVICE_SAMPLE_RATE), frm3(2550*k, DEVICE_SAMPLE_RATE);
    //      И   240  2250  3200
    //altoolset::SinGenerator frm1(240*k, DEVICE_SAMPLE_RATE), frm2(2250*k, DEVICE_SAMPLE_RATE), frm3(3200*k, DEVICE_SAMPLE_RATE);
    //      Ы   300  1480  2230
    //altoolset::SinGenerator frm1(300*k, DEVICE_SAMPLE_RATE), frm2(1480*k, DEVICE_SAMPLE_RATE), frm3(2230*k, DEVICE_SAMPLE_RATE);

    //auto generator = lettersMap["У"];
    auto& gen = lettersMap["У"];
    return play_generator(&*gen);
}

int main(int argc, char** argv)
{
    altoolset::initSinPrecalc();

    po::options_description desc("General options");
    std::string runType;
    desc.add_options()
        ("help,h", "Print this text")
        ("devices", "List of available devices")
        ("type,t", po::value(&runType), "Command type: wav|file, sin, floatsin, composite");

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
        }
        else if (runType == "floatsin") {
            desc.add(floatSinDesc);
            po::store(po::parse_command_line(argc, argv, desc), vm);
            po::notify(vm);
        }
        else if (runType == "composite") {
            // no additional arguments
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

    if (vm.count("help")) {
        // nop, just fallthrought
    }
    else if (runType == "sin") {
        if (!vm.count("freq")) {
            std::cerr << desc << std::endl;
            return 1;
        }
        auto res = QueueSinPlay(genFrequency, std::chrono::seconds(20));
        std::cout << res << std::endl;
        return 0;
    }
    else if (runType == "floatsin") {
        if (!vm.count("leftfreq") || !vm.count("rightfreq") || !vm.count("period")) {
            std::cerr << desc << std::endl;
            return 1;
        }
        std::cout << "Floating sinusoidal generation is not supported yet" << std::endl;
        auto res = QueueFloatingPlay(genFloatFrequency.leftFreq, genFloatFrequency.rightFreq,
                          (float)genFloatFrequency.period, std::chrono::seconds(60));
        std::cout << res << std::endl;
        return 0;
    }
    else if (runType == "composite") {
        auto res = QueueCompositePlay(genFrequency, std::chrono::seconds(20));
        std::cout << res << std::endl;
        return 0;
    }
    //desc.add(wavDesc).add(sinDesc).add(floatSinDesc);
    std::cout << desc << std::endl;
    return 0;
}
