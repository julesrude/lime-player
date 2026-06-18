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
    juce::Rectangle<float> drawingBounds_;

    juce::Path barTrianglesPath_;
    float triangleSize_ = 4.0f;
    
    juce::Colour linesColour_;
    float thickLineThickness_ = 1.0f;
    int numMajorDivisions_ = 5;
    int numMinorDivisions_ = 30;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundWavePath)
};
