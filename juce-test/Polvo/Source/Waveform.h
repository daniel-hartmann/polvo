/*
  ==============================================================================

    Waveform.h
    Created: 5 May 2024 8:00:23pm
    Author:  daniel

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Config.h"

class Waveform : public juce::Component, private juce::ChangeListener,
    private juce::Timer
{
public:
    Waveform();
    ~Waveform() override;

    enum DisplayMode {
        Buffer,
        File
    };

    void paint(juce::Graphics& g) override;
    void resized() override;

    void paintFromBuffer(juce::Graphics& g);
    void paintFromFile(juce::Graphics& g);

    void setFile(const juce::File& file);

    void setDisplayMode(DisplayMode mode);

    int16_t buffer[BUFFER_SIZE];

private:
    juce::File file;
    DisplayMode currentMode = Buffer;
    bool paintedFromFile = false;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Waveform)
};
