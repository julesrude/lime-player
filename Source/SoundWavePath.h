/*
  ==============================================================================

    SoundWavePath.h
    Created: 19 Mar 2026 1:44:51pm
    Author:  Yuliia Ruda

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class SoundWavePath  : public juce::Component
{
public:
    SoundWavePath(juce::Colour lines);
    ~SoundWavePath() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Colour linesColour;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundWavePath)
};
