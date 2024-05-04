#pragma once

#include <JuceHeader.h>
#include <math.h>
#include <pulse/pulseaudio.h>
#include "AnalysisData.h"

#define BUFFER_SIZE 1024

class LoudnessMeter : public juce::Component, private juce::Timer
{
public:
    LoudnessMeter();

    const double reference_level = 32768.0;

    void paint(juce::Graphics&) override;
    void resized() override;
    void startTimers(pa_mainloop *mainloop);

    struct Meter {
        double momentary;
        double shortTerm;
        int16_t buffer[BUFFER_SIZE];
    };
    Meter meter;

private:
    pa_mainloop *mainloop;

    unsigned int momentaryCounter = 0;
    unsigned int shortTermCounter = 0;

    void timerCallback() override;
    void drawLevel(juce::Graphics& g, int xOffset);
    double calculateRMS(int16_t *buffer, size_t length);
    double calculateLUFS(double rms);

    int positionForLevel(double level) const noexcept
    {
        return int(std::round(juce::jmap(level, maxLUFS, minLUFS, maxPos, minPos)));
    }

    static constexpr double maxLUFS = 0.0;
    static constexpr double minLUFS = -60.0;
    static constexpr double stepLUFS = 7.0;

    double maxPos;
    double minPos;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoudnessMeter)
};
