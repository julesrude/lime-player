/*
  ==============================================================================

    SoundWavePath.h
    Created: 19 Mar 2026 1:44:51pm
    Author:  Yuliia Ruda

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "YellowPlayerLookAndFeel.h"

//==============================================================================
/*
*/
class SoundWavePath  : public juce::Component,
                       public juce::ChangeListener
{
public:
    SoundWavePath(juce::AudioThumbnail &thumbnail, const int channel);
    ~SoundWavePath() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setNewSongLength();
    void resetTimeBounds(const float minSec, const float maxSec);
    void calculateDivisions();
    
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    
    void setSourceForWaveform (juce::InputSource *source);

private:
    juce::Rectangle<float> drawingBounds_;

    juce::Path trianglesPath_;
    juce::Path majorBarsPath_;
    juce::Path minorBarsPath_;
    float triangleSize_ = 4.0f;
    
    juce::Colour linesColour_ = YellowPlayerColours::white;
    float thickLineThickness_ = 1.0f;
    int numMajorDivisions_ = 5;
    int numMinorDivisions_ = 30;

    juce::AudioThumbnail &thumbnail_;
    int channel_;

    int majorDivisionsIntervalsSec_ = 0;
    float minSec_ = 0;
    float maxSec_ = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundWavePath)
};
