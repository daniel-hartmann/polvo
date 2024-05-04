/*
  ==============================================================================

    PolvoLookAndFeel.h
    Created: 3 May 2024 11:29:00pm
    Author:  daniel

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class PolvoLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override;
};

