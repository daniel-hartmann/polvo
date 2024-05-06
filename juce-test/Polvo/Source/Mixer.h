/*
  ==============================================================================

    Mixer.h
    Created: 5 May 2024 3:42:41pm
    Author:  daniel

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Config.h"

#define NUM_TRACKS 5

class Mixer : public juce::AudioAppComponent
{
public:
    Mixer(juce::AudioDeviceManager* deviceManager);
    ~Mixer() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void playPause();
    void setGain(int channelIndex, float gain);
    void loadAudioFile(int channelIndex, const juce::File& audioFile,
                       float initialGain = 1.0f);
    void removeAllInputs();

private:
    juce::AudioDeviceManager* deviceManager;
    juce::AudioFormatManager formatManager;
    juce::MixerAudioSource mixer;
    juce::AudioSourcePlayer sourcePlayer;

    std::unique_ptr<juce::AudioFormatReaderSource> readerSources[NUM_TRACKS];
    juce::AudioTransportSource transportSources[NUM_TRACKS];
    std::vector<float> channelGains;
};
