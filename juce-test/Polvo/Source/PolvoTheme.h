/*
  ==============================================================================

    PolvoTheme.h
    Created: 3 May 2024 11:27:42pm
    Author:  daniel

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace PolvoColor
{
const juce::Colour lightYellow = juce::Colour(0xffd2b48c);
const juce::Colour lightGray = juce::Colour(0xffc0c0c0);
const juce::Colour orange = juce::Colour(0xffcf702d);
const juce::Colour brown = juce::Colour(0xff8b4513);
const juce::Colour purple = juce::Colour(0xff4b0082);
const juce::Colour cyan = juce::Colour(0xff008080);
const juce::Colour beige = juce::Colour(0xffdeb887);
const juce::Colour silver = juce::Colour(0xffb0c4de);
const juce::Colour dark = juce::Colour(0xff1a1a1a);
const juce::Colour lessDark = juce::Colour(0xff2b2b2b);
const juce::Colour darkBrown = juce::Colour(0xff2b1f16);
const juce::Colour darkBlue = juce::Colour(0xff0d1c33);
}


class PolvoLookAndFeel : public juce::LookAndFeel_V4
{
public:
    PolvoLookAndFeel()
    {
        setDefaultSansSerifTypefaceName("Arimo");
    }

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (!slider.isEnabled()) {
            slider.setColour(juce::Slider::thumbColourId,
                             juce::Colour(0xffa9a9a9)); // Grayish thumb color
            slider.setColour(juce::Slider::trackColourId,
                             juce::Colour(0xff808080)); // Grayish track color
            slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xffd3d3d3));

        } else {
            slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffffa500));
            slider.setColour(juce::Slider::trackColourId, juce::Colour(0xff8b4513));
            slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xffa0a0a0));
        }

        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos,
                                         maxSliderPos, style, slider);
    }

    juce::Font getTextButtonFont(juce::TextButton& button, int buttonHeight) override
    {
        return juce::Font(10.0f, juce::Font::bold);
    }
};
