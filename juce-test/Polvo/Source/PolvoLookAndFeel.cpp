/*
  ==============================================================================

    PolvoLookAndFeel.cpp
    Created: 3 May 2024 11:27:42pm
    Author:  daniel

  ==============================================================================
*/

#include "PolvoLookAndFeel.h"

void PolvoLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width,
                                        int height,
                                        float sliderPos, float minSliderPos, float maxSliderPos,
                                        const juce::Slider::SliderStyle style, juce::Slider& slider)
{


  if (!slider.isEnabled()) {
    // Draw with disabled color
    // slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffffa500));
    // slider.setColour(juce::Slider::trackColourId, juce::Colour(0xff8b4513));
    // slider.setColour(juce::Slider::backgroundColourId, juce::Colour(0xffa0a0a0));

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
