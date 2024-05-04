#pragma once

#include <JuceHeader.h>
#include <math.h>
#include <pulse/pulseaudio.h>

#define BUFFER_SIZE 1024

class Vectorscope : public juce::Component, private juce::Timer
{
public:
    Vectorscope();

    void paint(juce::Graphics&) override;
    void resized() override;

    int16_t buffer[BUFFER_SIZE];

private:
    pa_mainloop *mainloop;

    void timerCallback() override;
    juce::Colour getColor(float hue);

    double maxPos;
    double minPos;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Vectorscope)
};
