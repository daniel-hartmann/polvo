#include "MainComponent.h"

const double MainComponent::referenceLevel = 32768.0;
double MainComponent::momentaryLoudness = 0.0;
double MainComponent::shortTermLoudness = 0.0;

//==============================================================================
MainComponent::MainComponent() : stems(), vectorscope(), loudnessMeter()
{
    setSize(800, 480);
    initPulse();

    loudnessMeter.startTimers(pa.mainloop);

    addAndMakeVisible(stems);
    addAndMakeVisible(vectorscope);
    addAndMakeVisible(loudnessMeter);
}

MainComponent::~MainComponent()
{
    if (pa.stream) {
        pa_stream_unref(pa.stream);
    }

    if (pa.context) {
        pa_context_disconnect(pa.context);
        pa_context_unref(pa.context);
    }

    if (pa.mainloop) {
        pa_mainloop_free(pa.mainloop);
    }

    free(pa.sink);
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{

}

void MainComponent::updateChildComponentBounds()
{
    juce::Rectangle<int> bounds = getLocalBounds();
    const int margin = 0;
    const int componentHeight = 200; // Adjust as needed
    int thirdWidth = bounds.getWidth() / 3;

    // Set bounds for stems component
    stems.setBounds(bounds.removeFromLeft(thirdWidth).reduced(margin));

    // Set bounds for vectorscope component
    vectorscope.setBounds(bounds.removeFromLeft(thirdWidth).reduced(margin));

    // Set bounds for loudnessMeter component
    loudnessMeter.setBounds(bounds.removeFromLeft(thirdWidth).reduced(margin));
}



void MainComponent::resized()
{
    updateChildComponentBounds();
}


//==============================================================================

/*
    Pulseaudio
*/


void MainComponent::initPulse()
{
    pa.mainloop = pa_mainloop_new();
    auto api = pa_mainloop_get_api(pa.mainloop);

    pa.context = pa_context_new(api, "Polvo");
    pa_context_set_state_callback(pa.context, [](pa_context * ctx, void* userdata) {
        static_cast<MainComponent*>(userdata)->handleContextState(ctx);
    }, this);

    if (pa_context_connect(pa.context, nullptr, PA_CONTEXT_NOFLAGS, nullptr) < 0) {
        printf("failed to connect");
    }
}

void MainComponent::handleContextState(pa_context* ctx)
{
    switch (pa_context_get_state(ctx)) {
    case PA_CONTEXT_READY:
        pa_operation_unref(
            pa_context_get_server_info(ctx, [](pa_context * ctx, const pa_server_info * info,
        void* userdata) {
            static_cast<MainComponent*>(userdata)->handleServerInfo(ctx, info);
        }, this)
        );
        break;

    case PA_CONTEXT_FAILED:
        printf("failure in context");
        break;

    default:
        break;
    }
}

void MainComponent::handleServerInfo(pa_context* ctx, const pa_server_info* info)
{
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16NE,
        .rate = 48000,
        .channels = 1
    };

    static const pa_buffer_attr ba = {
        .maxlength = BUFFER_SIZE,
        .tlength = (unsigned int) - 1,
        .prebuf = (unsigned int) - 1,
        .minreq = (unsigned int) - 1,
        .fragsize = BUFFER_SIZE
    };

    if (!pa.sink && asprintf(&pa.sink, "%s.monitor", info->default_sink_name) < 0) {
        printf("failure in asprintf()");
    }

    printf("using sink %s", pa.sink);

    pa.stream = pa_stream_new(ctx, "Input", &ss, nullptr);
    pa_stream_set_state_callback(pa.stream, [](pa_stream * stream, void* userdata) {
        static_cast<MainComponent*>(userdata)->handleStreamState(stream);
    }, this);
    pa_stream_set_read_callback(pa.stream, [](pa_stream * stream, size_t length,
    void* userdata) {
        static_cast<MainComponent*>(userdata)->handleStreamRead(stream, length);
    }, this);

    if (pa_stream_connect_record(pa.stream, pa.sink, &ba, PA_STREAM_ADJUST_LATENCY) < 0) {
        printf("failed to connect input stream");
    }
}

void MainComponent::handleStreamState(pa_stream* stream)
{
    if (pa_stream_get_state(stream) == PA_STREAM_FAILED) {
        printf("failure in input stream");
    }
}

void MainComponent::handleStreamRead(pa_stream* stream, size_t length)
{
    static size_t bufferIndex = 0;

    const void* data;
    const int numChannels = pa_stream_get_sample_spec(stream)->channels;
    const int numSamples = length / (2 * numChannels); // Assuming 16-bit samples



    if (pa_stream_peek(stream, &data, &length) < 0) {
        printf("failed to read fragment");
    }

    if (data) {
        const int16_t* audioSamples = static_cast<const int16_t*>(data);

        // Iterate over audio samples and populate buffers
        for (int i = 0; i < length / 2; ++i) {
            loudnessMeter.meter.buffer[bufferIndex] = audioSamples[i];
            vectorscope.buffer[bufferIndex] = audioSamples[i];
            stems.buffer[bufferIndex] = audioSamples[i];

            // Increment buffer index
            bufferIndex++;

            // Reset buffer index and trigger repaint if buffer is full
            if (bufferIndex == BUFFER_SIZE) {
                bufferIndex = 0;
                stems.processAudioData(BUFFER_SIZE);
                repaint();
            }
        }

    }

    if (length > 0 && pa_stream_drop(stream)) {
        printf("failed to drop fragment");
    }
}

