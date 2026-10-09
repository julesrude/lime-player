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

class PlayPositionMarker : public juce::Component
{
public:
    PlayPositionMarker() : juce::Component() {
        setInterceptsMouseClicks(false, false);
    };

    virtual void paint (juce::Graphics& g) override {
        g.setFillType(fill_);
        g.fillPath(path_);
    }

    void setPath (const juce::Path &p) {
        path_.clear();
        path_ = p;
    }

    void moveCentreTo (const int x) {
        markerPosX_ = juce::jlimit(horisontalMargin_,
                                   getWidth() - horisontalMargin_,
                                   x + (int)positionOffset_);

        juce::Path path;
        const float top = 0;

        // Draw vertical line
        path.addRectangle(juce::Rectangle<float>(markerThickness_, getHeight())
                          .withCentre({(float)markerPosX_, top + getHeight() / 2.0f}));

        // Draw triangle at the top of the vertical line (same style as major divisions)
        path.addTriangle(
                         markerPosX_,                 top + triangleSize_,
                         markerPosX_ - triangleSize_, top,
                         markerPosX_ + triangleSize_, top
                         );

        setPath(path);
        
        repaint();
    }

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

    float triangleSize_ = 4.0f;
    float markerThickness_ = 2.0f;
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
    float triangleSize_ = 4.0f;

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

    // Graphics constants
    const float thickLineThickness_ = 1.0f;
    const float horisontalMargin_ = 15.0f; // leaves room so time stamps and triangles aren't clipped
    const float verticalMargin_ = 20.0f;
    const juce::Colour linesColour_ = YellowPlayerColours::white;
    const juce::Colour playPositionMarkerColour_ = juce::Colour(0xffff5252);
    
    juce::Point<float> initialClickPosition_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundWavePath)
};
