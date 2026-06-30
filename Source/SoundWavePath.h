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

    // - RESIZING: -
    // To be used when loading a new song
    void setNewSongLength();

    // Reset time bounds, reset scale pixels, repaint() at the end
    void resetTimeBounds(const float minSec, const float maxSec);
    
    // Calculate how many minor and major bars (divisions) should be there, and their starting positions
    void calculateDivisions();
    // - - - - - - -

    void setSourceForWaveform (juce::InputSource *source);
    
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    void mouseWheelMove (const juce::MouseEvent& event,
                         const juce::MouseWheelDetails& wheel) override;

private:
    juce::Rectangle<float> soundwaveBounds_;

    juce::Path trianglesPath_;
    juce::Path majorBarsPath_;
    juce::Path minorBarsPath_;
    float triangleSize_ = 4.0f;
    
    juce::Colour linesColour_ = YellowPlayerColours::white;
    // #e8878b rgba(232, 135, 139, 1)
    float thickLineThickness_ = 1.0f;
    int numMajorDivisions_ = 5;
    int numMinorDivisions_ = 30;

    juce::AudioThumbnail &thumbnail_;
    int channel_;

    int majorDivisionsIntervalsSec_ = 0;
    juce::Range<float> secRange_ {0, 0};
    
    struct TimeStamp { juce::String text; juce::Rectangle<float> bounds; };
    std::vector<TimeStamp> timeStamps_{20};
    
    YellowPlayerLookAndFeel lookAndFeel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundWavePath)
};
