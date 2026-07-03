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
    void calculateResizedDivisions();
    
    std::function<void(juce::Range<double>)> onZoom;
    // - - - - - - -

    void setSourceForWaveform (juce::InputSource *source);
    
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    void mouseWheelMove (const juce::MouseEvent& event,
                         const juce::MouseWheelDetails& wheel) override;

    // Set the current playback position in seconds
    void setCurrentTime(double timeInSeconds);

private:

    void recreatePlayPositionMarker();

    // Helpers, convert between a time in seconds and X within soundwaveBounds_
    float timeToX(double sec) const;
    double xToTime(float x) const;

    juce::Rectangle<float> soundwaveBounds_;
    juce::AudioThumbnail &thumbnail_;
    int channel_;

    // Vertical bars and their timestamps
    juce::Path trianglesPath_;
    juce::Path majorBarsPath_;
    juce::Path minorBarsPath_;
    float triangleSize_ = 4.0f;

    double majorDivisionsIntervalsSec_ = 0;
    double minorDivisionsIntervalsSec_ = 0;
    juce::Range<double> secRange_ {0, 0};

    struct TimeStamp { juce::String text; juce::Rectangle<float> bounds; };
    static constexpr size_t maxTimeStamps_ = 20; // to be reserved up front to avoid reallocations
    std::vector<TimeStamp> timeStamps_;

    // Current time marker
    juce::Path playPositionMarkerPath_;
    double currentTime_ = 0.0;

    // Graphics constants
    const float thickLineThickness_ = 1.0f;
    const float horisontalMargin_ = 15.0f; // leaves room so time stamps and triangles aren't clipped
    const float verticalMargin_ = 20.0f;
    const juce::Colour linesColour_ = YellowPlayerColours::white;
    const juce::Colour playPositionMarkerColour_ = juce::Colour(0xffff5252);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundWavePath)
};
