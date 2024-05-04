#include "LoudnessMeter.h"

LoudnessMeter::LoudnessMeter()
{
    meter.momentary = -250.0;
    meter.shortTerm = -250.0;
}

void LoudnessMeter::startTimers(pa_mainloop *mainloop)
{
    this->mainloop = mainloop;
    startTimer(50);
}

void LoudnessMeter::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds();
    const int xOffset = 50;

    // Draw the scale markings and labels on the left side
    g.setFont(11.0f);

    for (float lufs = maxLUFS; lufs >= minLUFS; lufs -= stepLUFS) {
        int y = positionForLevel(lufs);

        g.setColour(lufs == 0.0f ? juce::Colour(120, 120, 120) : juce::Colour(200, 200, 200));
        g.fillRect(xOffset, y, 30, 1); // Adjust the x-coordinate (10) for scale position

        g.setColour(juce::Colour(100, 100, 100));
        g.drawSingleLineText(juce::String(int(lufs)), xOffset + 35, y + 4,
                             juce::Justification::left); // Adjust the x-coordinate (45) for label position
    }

    // Draw the level bar
    drawLevel(g, xOffset);

    // Draw LUFS strings
    int largeFontSize = 28;
    int smallFontSize = 13;
    int labelOffset = xOffset + 85;
    int yOffset = 40;

    juce::Font boldFont(largeFontSize, juce::Font::bold);

    g.setFont(boldFont);
    g.setColour(juce::Colour(0xff8B4513));

    juce::String momentaryText = juce::String(meter.momentary, 1);
    juce::String shortTermText = juce::String(meter.shortTerm, 1);

    if (meter.momentary <= -250.0) {
        momentaryText = " -INF";
    }

    if (meter.shortTerm <= -250.0) {
        shortTermText = " -INF";
    }

    g.drawSingleLineText(momentaryText, labelOffset, yOffset);
    yOffset += largeFontSize - 10;

    g.setFont(smallFontSize);
    g.drawSingleLineText("MOMENTARY", labelOffset, yOffset);
    yOffset += smallFontSize;
    yOffset += 50; // spacer

    g.setFont(boldFont);
    g.drawSingleLineText(shortTermText, labelOffset, yOffset);
    g.setFont(smallFontSize);
    yOffset += largeFontSize - 10;
    g.drawSingleLineText("SHORT TERM", labelOffset, yOffset);
}

void LoudnessMeter::drawLevel(juce::Graphics& g, int xOffset)
{
    int x = xOffset;
    double peak = -14.0;

    int y = positionForLevel(meter.momentary);
    int y0 = positionForLevel(peak);

    if (y < getHeight()) {
        g.setColour(juce::Colour(210, 180, 140));
        g.fillRect(x, y, 14, getHeight() - y);
    }

    if (meter.momentary > peak) {
        // Draw the red part (for meter.momentary > -14.0)
        g.setColour(juce::Colour(204, 76, 63));
        g.fillRect(x, y, 14, y0 - y);

    }
}


void LoudnessMeter::resized()
{
    minPos = float(getHeight()) - 30.0f;
    maxPos = 10.0f;
}

void LoudnessMeter::timerCallback()
{
    int interval = getTimerInterval();

    momentaryCounter += interval;
    shortTermCounter += interval;

    if (pa_mainloop_iterate(mainloop, false, NULL) < 0) {
        printf("Failed to iterate main loop\n");
    }

    if (momentaryCounter == 400) {
        double rms = calculateRMS(meter.buffer, BUFFER_SIZE);
        meter.momentary = calculateLUFS(rms);

        momentaryCounter = 0;
        repaint();
    }

    if (shortTermCounter == 3000) {
        double rms = calculateRMS(meter.buffer, BUFFER_SIZE);
        meter.shortTerm = calculateLUFS(rms);

        shortTermCounter = 0;
        repaint();
    }
}

// Calculate the Root Mean Square (RMS) from audio buffer
double LoudnessMeter::calculateRMS(int16_t *buffer, size_t length)
{
    double sum = 0.0;

    for (size_t i = 0; i < length; i++) {
        sum += buffer[i] * buffer[i];
    }

    double rms = sqrt(sum / length);
    return rms;
}

// Calculate LUFS from RMS value
double LoudnessMeter::calculateLUFS(double rms)
{
    double adjusted_rms = fmax(rms, 1e-8);  // Avoid log(0) issues
    double adjusted_reference = fmax(reference_level, 1e-8);

    double lufs = 20 * log10(adjusted_rms / adjusted_reference);
    return lufs;
}

