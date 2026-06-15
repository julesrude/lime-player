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
    
    juce::Path barTrianglesPath;
    
    juce::Rectangle<float> drawingBounds;
    
    float thickLineThickness = 2.0f;
    int numMajorDivisions = 5;
    int numMinorDivisions = 30;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundWavePath)
};
