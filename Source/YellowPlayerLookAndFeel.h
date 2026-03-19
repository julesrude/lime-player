/*
  ==============================================================================

    YellowPlayerLookAndFeel.h
    Created: 19 Mar 2026 11:47:34am
    Author:  Yuliia Ruda

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/

class YellowPlayerLookAndFeel :  public juce::LookAndFeel_V4
{
public:
    YellowPlayerLookAndFeel();
    
    juce::Label* createSliderTextBox (juce::Slider& slider) override;
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos,
                                           float minSliderPos,
                                           float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    
    void drawLinearSliderOutline(juce::Graphics&, int, int, int, int,
                                 juce::Slider::SliderStyle, juce::Slider&) override {}

    juce::Colour limeColor = juce::Colour(251, 253, 130);
    juce::Colour midGreyColor = juce::Colour(180, 180, 180);
    juce::Colour blackColor { 0xff1f2937 };
    juce::Colour darkGreyColor { 0xff374151 };
    juce::Colour lightGreyColor { 0xffd1d5db };
    juce::Colour whiteColor { 0xfff9fafb };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(YellowPlayerLookAndFeel)
};
