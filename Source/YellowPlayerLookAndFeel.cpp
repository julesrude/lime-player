/*
  ==============================================================================

    YellowPlayerLookAndFeel.cpp
    Created: 19 Mar 2026 11:47:34am
    Author:  Yuliia Ruda

  ==============================================================================
*/

#include <JuceHeader.h>
#include "YellowPlayerLookAndFeel.h"

//==============================================================================

YellowPlayerLookAndFeel::YellowPlayerLookAndFeel()
{
    // Sliders feel
    setColour(juce::Slider::backgroundColourId, YellowPlayerColours::lightGrey);
    setColour(juce::Slider::trackColourId, YellowPlayerColours::lime);
    
    // Other elements
    setColour(juce::Label::textColourId, YellowPlayerColours::lime);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

}

juce::Label* YellowPlayerLookAndFeel::createSliderTextBox (juce::Slider& slider)
{
    juce::Label* returnLabel = LookAndFeel_V4::createSliderTextBox(slider);
    returnLabel->setColour(juce::Label::textColourId, juce::Colours::darkgrey);

    return returnLabel;
}

void YellowPlayerLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                       float sliderPos,
                                       float minSliderPos,
                                       float maxSliderPos,
                                       const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (slider.isBar() && slider.isHorizontal())
    {
        g.setColour(slider.findColour (juce::Slider::backgroundColourId));
        g.fillRoundedRectangle(x, y, width, height, 3.0f);
        
        g.setColour (slider.findColour (juce::Slider::trackColourId));
        g.fillRoundedRectangle(x, y, sliderPos, height, 3.0f);
    }
    else
    {
        juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}


//============================== SHAPES ========================================

juce::Path YellowPlayerLookAndFeel::plusShape(float size, float thickness) {
    juce::Path plus;

    // horisontal long rectangle
    plus.addRectangle(0, size / 2.0f - thickness / 2.0f, size, thickness);
    // vertical long rectangle
    plus.addRectangle(size / 2.0f - thickness / 2.0f, 0, thickness, size);
    
    return plus;
}

juce::Path YellowPlayerLookAndFeel::pauseShape(float height, float width, float thickness) {
    juce::Path pause;

    // two long vertical rectangles
    pause.addRectangle(0, 0, thickness, height);
    pause.addRectangle(width - thickness, 0, thickness, height);
    
    return pause;
}

juce::Path YellowPlayerLookAndFeel::playShape(float height, float width) {
    juce::Path play;

    // two long vertical rectangles
    play.addTriangle(0, 0, width, height / 2.0, 0, height);
    
    return play;
}
