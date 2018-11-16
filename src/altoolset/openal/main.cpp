#include <iostream>
#include <stack>
#include <vector>
#include <thread>
#include <AL/al.h>
#include <AL/alc.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "altoolset/altoolset.hpp"
#include "altoolset/generators/sin_generator.hpp"
#include "altoolset/generators/floating_sin_generator.hpp"

namespace po=boost::program_options;

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

void QueueFloatingPlay(altoolset::openal::Device& dev, ALCint freqMin, ALCint freqMax, ALfloat period, std::chrono::milliseconds time, std::shared_ptr<altoolset::openal::Context> ctx)
{
    const auto deviceRate = dev.getDeviceRate();
    std::cout << "Creating frequency wave..." << std::endl;
    altoolset::SinGenerator frequencyWave(period, deviceRate);
    frequencyWave.setOffset(M_PI/4);
    frequencyWave.init();

    std::cout << "Creating floating sin generator " << freqMin << "<->" << freqMax << " ..." << std::endl;
    altoolset::FloatingSinGenerator generator(freqMin, freqMax, &frequencyWave, deviceRate);
    generator.init();
    std::cout << "Creating queue player..." << std::endl;
    auto player = ctx->createQueuePlayer(generator);
    std::cout << "Playing..." << std::endl;
    player->play();
    std::this_thread::sleep_for(time);
    player->stop();
}

void QueueSinPlay(altoolset::openal::Device& dev, ALCint frequency, std::chrono::milliseconds time, std::shared_ptr<altoolset::openal::Context> ctx)
{
    altoolset::SinGenerator generator(frequency, dev.getDeviceRate());
    generator.init();
    auto player = ctx->createQueuePlayer(generator);
    player->play();
    std::this_thread::sleep_for(time);
    player->stop();
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

    try {
        po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);
        if (vm.count("help")) {
            // nop, just fallthrought
        }
        else if (vm.count("devices")) {
            std::cout << "Devices list:" << std::endl;
            for (auto device : devicesList) {
                std::cout << "  " << device << std::endl;
            }
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
            QueueSinPlay(device, genFrequency, std::chrono::seconds(20), ctx);
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
            float period = (float)genFloatFrequency.period;
            std::cerr << "Period: " << period << std::endl;
            QueueFloatingPlay(device, genFloatFrequency.leftFreq, genFloatFrequency.rightFreq,
                              (ALfloat)period, std::chrono::seconds(60), ctx);
            return 0;
        }
        else if (runType == "wav" || runType == "file") {
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
            WavPlay(ctx, 1, fp.c_str());
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
