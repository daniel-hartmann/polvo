#include "Vectorscope.h"

Vectorscope::Vectorscope()
{
}


void Vectorscope::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds();
    const int quarterWidth = bounds.getWidth() / 4;
    const int xOffset = quarterWidth * 2;
    const int height = bounds.getHeight();

    g.setColour(juce::Colours::white); // Set the drawing color to white

    // Render audio data as points on the vectorscope
    for (int i = 0; i < BUFFER_SIZE; i += 2) {
        float x = buffer[i] / 30000.0f;
        float y = buffer[i + 1] / 30000.0f;

        // Calculate the point position relative to the component bounds
        int px = xOffset + static_cast<int>(x * quarterWidth);
        int py = height / 2 + static_cast<int>(y * height / 2);

        g.setColour(getColor(std::sqrt(x * x + y * y) * 180.0f));

        // Draw a point at the calculated position
        // g.drawRect(px, py, 1, 1);

        // int pointDiameter = 2;
        // g.fillEllipse(px - pointDiameter / 2, py - pointDiameter / 2,
        //               pointDiameter, pointDiameter);

        int pointSize = 4;
        // Draw a diamond shape at the calculated position
        juce::Path diamondPath;
        diamondPath.startNewSubPath(px - pointSize / 2, py); // Top point
        diamondPath.lineTo(px, py - pointSize / 2); // Right point
        diamondPath.lineTo(px + pointSize / 2, py); // Bottom point
        diamondPath.lineTo(px, py + pointSize / 2); // Left point
        diamondPath.closeSubPath();

        g.fillPath(diamondPath);

    }
}

juce::Colour Vectorscope::getColor(float hue)
{
    // Normalize hue within the range [0, 360)
    if (hue >= 360.0f) {
        hue -= 360.0f;
    }

    // Map hue to the range [0, 6)
    hue /= 60.0f;
    int i = static_cast<int>(std::floor(hue)) % 6;
    float f = hue - std::floor(hue);
    float q = 1.0f - f;

    // Adjusted color parameters for desert tones
    float saturation = 0.3f + 0.3f * std::cos(hue * juce::MathConstants<float>::pi /
                       180.0f);
    float brightness = 0.8f + 0.2f * std::sin(hue * juce::MathConstants<float>::pi /
                       180.0f);

    switch (i) {
    case 0: return juce::Colour::fromHSV(40.0f + f / 20, saturation * 0.8f,
                                             brightness * 1.0f,
                                             1.0f);
        break;

    case 1: return juce::Colour::fromHSV(35.0f + f / 20, saturation * 0.7f,
                                             brightness * 0.9f,
                                             1.0f);
        break;

    case 2: return juce::Colour::fromHSV(30.0f + f / 20, saturation * 0.6f,
                                             brightness * 0.8f,
                                             1.0f);
        break;

    case 3: return juce::Colour::fromHSV(25.0f + f / 20, saturation * 0.5f,
                                             brightness * 0.7f,
                                             1.0f);
        break;

    case 4: return juce::Colour::fromHSV(20.0f + f / 20, saturation * 0.4f,
                                             brightness * 0.6f,
                                             1.0f);
        break;

    case 5: return juce::Colour::fromHSV(15.0f + f / 20, saturation * 0.3f,
                                             brightness * 0.5f,
                                             1.0f);
        break;

    default: return juce::Colours::white; // Fallback color (shouldn't be reached)
    }
}





void Vectorscope::resized()
{
    minPos = float(getHeight()) - 30.0f;
    maxPos = 10.0f;
}


void Vectorscope::timerCallback()
{
    int interval = getTimerInterval();
}
