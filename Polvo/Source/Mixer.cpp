/*
  ==============================================================================

    Mixer.cpp
    Created: 5 May 2024 3:42:41pm
    Author:  daniel

  ==============================================================================
*/

#include "Mixer.h"

Mixer::Mixer(juce::AudioDeviceManager* deviceManager): deviceManager(deviceManager)
{
    formatManager.registerBasicFormats();

    // Initialize transport sources for each channel
    for (int channelIndex = 0; channelIndex < NUM_TRACKS; ++channelIndex) {
        transportSources[channelIndex].setSource(nullptr);
        mixer.addInputSource(&transportSources[channelIndex], false);
    }

    // Set the mixer as the source for the AudioSourcePlayer
    sourcePlayer.setSource(&mixer);

    // Add the AudioSourcePlayer as an audio callback to the device manager
    deviceManager->addAudioCallback(&sourcePlayer);

}

void Mixer::setGain(int channelIndex, float gain)
{
    std::cout << "Mixer::setGain(" << channelIndex << ", " << gain << ")" << std::endl;

    if (channelIndex >= 0 && channelIndex < NUM_TRACKS) {
        std::cout << std::endl << "* Changing gain for track " << channelIndex << "...\t";
        transportSources[channelIndex].setGain(gain);
        std::cout << "OK" << std::endl;
    }
}

void Mixer::loadAudioFile(int channelIndex, const juce::File& file, float gain)
{
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader != nullptr) {
        // Release existing source if any
        transportSources[channelIndex].setSource(nullptr);

        std::unique_ptr<juce::AudioFormatReaderSource> readerSource(new
                juce::AudioFormatReaderSource(reader.release(), false));

        // Set the reader source to the transport source
        // transportSources[channelIndex].setSource(readerSource.get(), 0, nullptr,
        //         reader->lengthInSamples);
        transportSources[channelIndex].setGain(gain);
        transportSources[channelIndex].setSource(readerSource.get());

        std::cout << "setting position to 0...\t";
        transportSources[channelIndex].setPosition(0.0f);
        std::cout << "OK" << std::endl;

        std::cout << "preparing to play...\t";
        transportSources[channelIndex].prepareToPlay(2,
                deviceManager->getCurrentAudioDevice()->getCurrentSampleRate());
        std::cout << "OK" << std::endl;

        // Keep ownership of the reader source in the transportSources array
        readerSources[channelIndex] = std::move(readerSource);


    } else {
        std::cerr << "Failed to create AudioFormatReader for file: " << file.getFullPathName()
                  << std::endl;

    }
}

void Mixer::playPause()
{
    for (int channelIndex = 0; channelIndex < NUM_TRACKS; ++channelIndex) {
        std::cout << std::endl << "======== TRACK: " << channelIndex << std::endl;
        std::cout << std::endl << "checking if transport source is playing...\t";

        if (transportSources[channelIndex].isPlaying()) {
            std::cout << "it IS playing" << std::endl;

            std::cout << "stopping...\t";
            transportSources[channelIndex].stop();
            std::cout << "OK" << std::endl;
        }

        std::cout << "it is NOT playing" << std::endl;

        std::cout << "starting...\t";
        transportSources[channelIndex].start();
        std::cout << "OK" << std::endl;
    }
}

void Mixer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void Mixer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixer.getNextAudioBlock(bufferToFill);
}

void Mixer::releaseResources()
{
    mixer.releaseResources();
}

void Mixer::removeAllInputs()
{
    // Stop and release all transport sources
    for (int channelIndex = 0; channelIndex < NUM_TRACKS; ++channelIndex) {
        transportSources[channelIndex].stop();
        transportSources[channelIndex].setSource(nullptr); // Release source
        readerSources[channelIndex].reset(); // Release reader source
    }
}

Mixer::~Mixer()
{
    removeAllInputs();

    deviceManager->removeAudioCallback(&sourcePlayer);
}

