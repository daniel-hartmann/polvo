#pragma once

#include <cmath>
#include <pulse/pulseaudio.h>
#include <JuceHeader.h>
#include "Stems.h"
#include "Vectorscope.h"
#include "LoudnessMeter.h"

#define BUFFER_SIZE 1024

static struct {
    char *sink;
    pa_mainloop *mainloop;
    pa_context *context;
    pa_stream *stream;
} pa;


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    static const double referenceLevel;
    static double momentaryLoudness;
    static double shortTermLoudness;
    int16_t buffer[BUFFER_SIZE];

    // Pulseaudio
    struct PAContext {
        char* sink = nullptr;
        pa_mainloop* mainloop = nullptr;
        pa_context* context = nullptr;
        pa_stream* stream = nullptr;
    };
    void initPulse();
    void handleContextState(pa_context* ctx);
    void handleServerInfo(pa_context* ctx, const pa_server_info* info);
    void handleStreamState(pa_stream* stream);
    void handleStreamRead(pa_stream* stream, size_t length);

private:
    //==============================================================================
    Stems stems;
    Vectorscope vectorscope;
    LoudnessMeter loudnessMeter;

    void updateChildComponentBounds();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
