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
#include "Utils.h"

class PlayPositionMarker : public juce::Component
{
public:
    PlayPositionMarker();

    void paint (juce::Graphics& g) override;

    void setPath (const juce::Path &p);

    // x is in this marker's bounds coordinates
    void moveCentreTo (const int x);

    void registerOffset(const float offset) { positionOffset_ = offset; }

    float getCentreX() { return markerPosX_; }
    
    void setFill(const juce::FillType & f) { fill_ = f; }
    
    void setHorisontalMargin(const int margin) { horisontalMargin_ = margin; }

private:
    juce::Path path_;
    juce::FillType fill_;
    int markerPosX_ = 0;
    int positionOffset_ = 0; // for click and drag, when click happened near the marker
    int horisontalMargin_ = 0;
};

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
   #if YELLOWPLAYER_SHOW_BOUNDS
    void paintOverChildren (juce::Graphics&) override;
   #endif
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

    bool hitTest(int x, int y) override {
        return true; // whole component bounds are clickable
    }

    bool isDragging() const { return isDragging_; }


    // Extra space on top of the waveform area for components in a non-compact view
    // (e.g. for a channel label drawn by the parent). No resized()/repaint() here
    void setExtraTopMargin(float margin) { extraTopMargin_ = margin; }

    std::function<void(const juce::MouseEvent &)> onClick;
    void mouseDown (const juce::MouseEvent &event) override;
    
    std::function<void(const juce::MouseEvent &)> onDrag;
    void mouseDrag (const juce::MouseEvent &event) override;

    std::function<void(const juce::MouseEvent &)> onMouseUp;
    void mouseUp (const juce::MouseEvent& event) override;

    // Called on mouse-up with the seek time in seconds, after the marker has been placed
    std::function<void(double)> onSeekRequested;

private:

    // Helpers, convert between a time in seconds and X within soundwaveBounds_
    float timeToX(double sec) const;
    double xToTime(float x) const;

    // Waveform
    juce::Rectangle<float> soundwaveBounds_;
    juce::AudioThumbnail &thumbnail_;
    int channel_;

    juce::Image waveformCache_;
    bool waveformCacheDirty_ = true;

    void renderWaveformToCache();

    // Vertical bars and their timestamps
    juce::Path trianglesPath_;
    juce::Path majorBarsPath_;
    juce::Path minorBarsPath_;

    double majorDivisionsIntervalsSec_ = 0;
    double minorDivisionsIntervalsSec_ = 0;
    juce::Range<double> secRange_ {0, 0};

    struct TimeStamp { juce::String text; juce::Rectangle<float> bounds; };
    static constexpr size_t maxTimeStamps_ = 20; // to be reserved up front to avoid reallocations
    std::vector<TimeStamp> timeStamps_;

    // Current time marker
    PlayPositionMarker playPositionMarkerPath_;
    double currentTime_ = 0.0;
    bool isDragging_ = false;

    void recreatePlayPositionMarker();

    float extraTopMargin_ = 0.0f; // set by the parent, see setExtraTopMargin()

    juce::Point<float> initialClickPosition_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundWavePath)
};
