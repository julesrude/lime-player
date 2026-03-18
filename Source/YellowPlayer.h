/*
  ==============================================================================

    YellowPlayer.h
    Created: 18 Mar 2026 8:21:57am
    Author:  Yuliia Ruda

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


class YellowPlayerLookAndFeel :  public juce::LookAndFeel_V4
{
public:
    YellowPlayerLookAndFeel()
    {
        // Sliders feel
        setColour(juce::Slider::backgroundColourId, juce::Colours::lightgrey);
        setColour(juce::Slider::trackColourId, limeColor);
        
        // Other elements
        setColour(juce::Label::textColourId, limeColor);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

    };
    
    juce::Label* createSliderTextBox (juce::Slider& slider) override {
        juce::Label* returnLabel = LookAndFeel_V4::createSliderTextBox(slider);
        returnLabel->setColour(juce::Label::textColourId, juce::Colours::darkgrey);

        return returnLabel;
    }
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos,
                                           float minSliderPos,
                                           float maxSliderPos,
                                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (slider.isBar() && slider.isHorizontal())
        {
            g.setColour(slider.findColour (juce::Slider::backgroundColourId));
            g.fillRoundedRectangle(x, y, width, height, 3.0f);
            
            g.setColour (slider.findColour (juce::Slider::trackColourId));
            g.fillRoundedRectangle(x, y, sliderPos, height, 3.0f);

        } else {
            juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        }

//        drawLinearSliderOutline (g, x, y, width, height, style, slider);
    }
    
    void drawLinearSliderOutline(juce::Graphics&, int, int, int, int,
                                 juce::Slider::SliderStyle, juce::Slider&) override {}


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(YellowPlayerLookAndFeel)

    juce::Colour limeColor = juce::Colour(251, 253, 130);
};

// //==============================================================================

class SoundLevelSlider : public juce::Slider {
public:
    SoundLevelSlider() : juce::Slider(juce::Slider::SliderStyle::LinearBar
                     , juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    {
        setRange(0.0, 100.0);
        setValue(50);
        setTextBoxIsEditable(false);
    };
    
    juce::String getTextFromValue (double value)
    {
        return juce::String(static_cast<int>(value)) + "%";
    }
};

//==============================================================================
/*
*/
class YellowPlayer  : public juce::Component
{
public:
    YellowPlayer();
    ~YellowPlayer() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (YellowPlayer)
    YellowPlayerLookAndFeel lookAndFeel;
    
    SoundLevelSlider slider;

    juce::Colour midGreyColor = juce::Colour(180, 180, 180);
};
