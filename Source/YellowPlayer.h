/*
  ==============================================================================

    YellowPlayer.h
    Created: 18 Mar 2026 8:21:57am
    Author:  Yuliia Ruda

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "YellowPlayerLookAndFeel.h"
#include "SoundWavePath.h"

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
    
    SoundLevelSlider topSlider_;
    SoundLevelSlider bottomSlider_;
    
    juce::ShapeButton pauseButton_;
    juce::ShapeButton playButton_;
    juce::ShapeButton plusButton_;
    
    const float controlsAreaHeight_ = 60.0f;
    const float helveticaMultiplier = 1.43; // calculated as (height / ascent);

    SoundWavePath topSoundWave_ = {YellowPlayerColours::white};
};
