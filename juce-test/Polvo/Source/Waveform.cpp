/*
  ==============================================================================

    Waveform.cpp
    Created: 5 May 2024 8:00:23pm
    Author:  daniel

  ==============================================================================
*/

#include "Waveform.h"

Waveform::Waveform() : formatManager(), thumbnailCache(5), thumbnail(512, formatManager,
            thumbnailCache)
{
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
}

Waveform::~Waveform()
{
    stopTimer();
}

void Waveform::paint(juce::Graphics& g)
{
    if (currentMode == Buffer) {
        paintFromBuffer(g);
        DBG("Drawing from Buffer");

    } else if (currentMode == File) {
        paintFromFile(g);
        DBG("Drawing from File");
    }
}

void Waveform::paintFromBuffer(juce::Graphics& g)
{
    if (buffer == nullptr || BUFFER_SIZE == 0) {
        return;
    }

    const float xScale = static_cast<float>(getWidth()) / BUFFER_SIZE;
    const float yScale = static_cast<float>(getHeight()) / 65536.0f;

    g.setColour(juce::Colours::white);

    juce::Path waveformPath;
    waveformPath.startNewSubPath(0, getHeight() / 2);

    for (int i = 0; i < BUFFER_SIZE; ++i) {
        float x = i * xScale;
        float y = (buffer[i] + 32768) * yScale;

        waveformPath.lineTo(x, y);
    }

    g.strokePath(waveformPath, juce::PathStrokeType(1.5f));
}

void Waveform::paintFromFile(juce::Graphics& g)
{
    g.setColour(juce::Colours::white);

    if (thumbnail.getNumChannels() > 0) {
        thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);

    } else {
        currentMode = Buffer;
    }
}

void Waveform::setFile(const juce::File& file)
{
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader != nullptr) {
        thumbnail.setSource(new juce::FileInputSource(file));
        repaint(); // Trigger repaint to update waveform
    }
}

void Waveform::setDisplayMode(DisplayMode mode)
{
    currentMode = mode;
    paintedFromFile = false;
    repaint();

    if (mode == Buffer) {
        stopTimer();

    } else {
        startTimer(40);
    }
}

void Waveform::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &thumbnail) {
        repaint();
    }
}

void Waveform::timerCallback()
{
    paintedFromFile = false;
    repaint();
}


void Waveform::resized()
{
}
