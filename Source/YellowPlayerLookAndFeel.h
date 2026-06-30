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

namespace YellowPlayerColours {
    const juce::Colour lime = juce::Colour(251, 253, 130);
    const juce::Colour midGrey = juce::Colour(180, 180, 180);
    const juce::Colour black { 0xff1f2937 };
    const juce::Colour darkGrey { 0xff374151 };
    const juce::Colour lightGrey { 0xffd1d5db };
    const juce::Colour white { 0xfff9fafb };
}

class YellowPlayerLookAndFeel :  public juce::LookAndFeel_V4
{
public:
    YellowPlayerLookAndFeel();
    
    juce::Label* createSliderTextBox (juce::Slider& slider) override;
    juce::Font getLabelFont (juce::Label&) override;

    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos,
                                           float minSliderPos,
                                           float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    void drawLinearSliderOutline(juce::Graphics&, int, int, int, int,
                                 juce::Slider::SliderStyle, juce::Slider&) override {}
    
    static juce::Path plusShape(float size, float thickness);
    static juce::Path pauseShape(float height, float width, float thickness);
    static juce::Path playShape(float height, float width);

    static juce::FontOptions getHelveticaForHeight(float height);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(YellowPlayerLookAndFeel)
};
