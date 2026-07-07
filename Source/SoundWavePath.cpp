/*
  ==============================================================================

    SoundWavePath.cpp
    Created: 19 Mar 2026 1:44:51pm
    Author:  Yuliia Ruda

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SoundWavePath.h"

namespace {
// The way these are used is, for the range that is above [i],
// the major bars interval is 1 step below ([i+1]), the minor bars interval is 3 steps below ([i+3])
const std::vector<juce::RelativeTime> zoomingTimeIntervals {
    juce::RelativeTime::minutes(9),
    juce::RelativeTime::minutes(3),
    juce::RelativeTime::minutes(1),
    juce::RelativeTime::seconds(30),
    juce::RelativeTime::seconds(10),
    juce::RelativeTime::seconds(5),
    juce::RelativeTime::seconds(2),
    juce::RelativeTime::seconds(1),
    juce::RelativeTime::milliseconds(250),
    juce::RelativeTime::milliseconds(100)
};

std::pair<juce::RelativeTime, juce::RelativeTime> majMinIntervalFromRange(float secRange) {
    juce::RelativeTime range{(double)secRange};
    
    // Stop 3 before the end so [i + 3] stays in bounds
    for (int i = 0; i < (int)zoomingTimeIntervals.size() - 3; ++i) {
        if (range > zoomingTimeIntervals[i]) {
            return {zoomingTimeIntervals[i + 1], zoomingTimeIntervals[i + 3]};
        }
    }
    return {*(zoomingTimeIntervals.end() - 3), *(zoomingTimeIntervals.end() - 1)};
}

template <typename Action>
void foreachVerticalBar(const juce::Rectangle<float> drawingBounds,
                        const float firstBar,
                        const float spaceBtwBars,
                        Action&& action) {

    // Number of bars will be 1 more than number of divisions
    for (int bar = 0; drawingBounds.getX() + firstBar + spaceBtwBars * bar <= drawingBounds.getRight(); ++bar) {
        float x = drawingBounds.getX() + firstBar + spaceBtwBars * bar;
        
        action(x);
    }
}
}

//==============================================================================
SoundWavePath::SoundWavePath(juce::AudioThumbnail &thumbnail, const int channel)
    : thumbnail_(thumbnail),
      channel_(channel)
{
    timeStamps_.reserve(maxTimeStamps_);
    thumbnail_.addChangeListener(this);
}

SoundWavePath::~SoundWavePath()
{
}

void SoundWavePath::paint (juce::Graphics& g)
{
    g.setColour (linesColour_);

    g.drawHorizontalLine(soundwaveBounds_.getCentreY(), soundwaveBounds_.getX(), soundwaveBounds_.getRight());
    
    g.setOpacity(0.15f);
    g.fillPath(minorBarsPath_);

    g.setOpacity(1.0f);
    g.fillPath(majorBarsPath_);
    g.fillPath(trianglesPath_);

    // Audio waveform
    if (waveformCacheDirty_)
        renderWaveformToCache();

    if (waveformCache_.isValid())
        g.drawImage(waveformCache_, soundwaveBounds_);

    // Time stamps on major bars
    g.setOpacity(0.5f);
    g.setFont(YellowPlayerLookAndFeel::getHelveticaForHeight(8));
    for (auto &timePoint : timeStamps_) {
        g.drawText(timePoint.text, timePoint.bounds, juce::Justification::centred);
    }

    // Draw current time marker if it's in the visible window
    if (currentTime_ >= secRange_.getStart() && currentTime_ <= secRange_.getEnd())
    {
        g.setColour(playPositionMarkerColour_);
        g.fillPath(playPositionMarkerPath_);
    }
}

void SoundWavePath::resized()
{
    soundwaveBounds_ = getLocalBounds().toFloat().reduced(horisontalMargin_, 20.0f).translated(0, 10.0f);
    calculateResizedDivisions();
    waveformCacheDirty_ = true;
}

void SoundWavePath::changeListenerCallback(juce::ChangeBroadcaster *source) {
    if (source == &thumbnail_) {
        waveformCacheDirty_ = true;
        repaint();
    }
}

void SoundWavePath::setSourceForWaveform(juce::InputSource *source) {
    thumbnail_.setSource (source);
    setNewSongLength();
}

void SoundWavePath::setNewSongLength() {
    resetTimeBounds(0.0f, thumbnail_.getTotalLength());
}

void SoundWavePath::resetTimeBounds(const float minSec, const float maxSec) {
    secRange_.setStart(minSec);
    secRange_.setEnd(maxSec);
    waveformCacheDirty_ = true;

    std::pair<juce::RelativeTime, juce::RelativeTime> intervals = majMinIntervalFromRange(secRange_.getLength());

    majorDivisionsIntervalsSec_ = intervals.first.inSeconds();
    minorDivisionsIntervalsSec_ = intervals.second.inSeconds();

    calculateResizedDivisions();
    repaint(); // TODO: not sure about it!
}

void SoundWavePath::calculateResizedDivisions() {
    const double start = secRange_.getStart();
    const double end = secRange_.getEnd();
    const double range = secRange_.getLength();
    
    trianglesPath_.clear();
    majorBarsPath_.clear();
    minorBarsPath_.clear();
    timeStamps_.clear();

    // Coordinates of first bars as defined inside soundwaveBounds_, so 0.0f == soundwaveBounds_.getX()
    float firstMajorDivision = 0.0f;
    float firstMinorDivision = 0.0f;

    // Default for when no song is loaded
    float spaceBtwMajorDivisions = soundwaveBounds_.getWidth() / 5.0f;
    float spaceBtwMinorDivisions = (soundwaveBounds_.getWidth() / 5.0f) / 6.0f;

    if (end > 0) {
        spaceBtwMajorDivisions = majorDivisionsIntervalsSec_ / range * soundwaveBounds_.getWidth();
        spaceBtwMinorDivisions = minorDivisionsIntervalsSec_ / range * soundwaveBounds_.getWidth();
        
        double firstMajorDivisionSec = std::ceil(start / majorDivisionsIntervalsSec_) * majorDivisionsIntervalsSec_;

        firstMajorDivision = (float)((firstMajorDivisionSec - start) / range ) * soundwaveBounds_.getWidth();
        spaceBtwMajorDivisions = (float)(majorDivisionsIntervalsSec_ / range ) * soundwaveBounds_.getWidth();

        int leftOffsetFromFirstMajor = 0;
        for (int i = 0; i < (majorDivisionsIntervalsSec_ / minorDivisionsIntervalsSec_); i++) {
            double candidate = firstMajorDivisionSec - i * minorDivisionsIntervalsSec_;
            if (candidate == start) {
                leftOffsetFromFirstMajor = i;
                break;
            } else if (candidate < start) {
                leftOffsetFromFirstMajor = i - 1;
                break;
            }

            if (i + 1 >=  majorDivisionsIntervalsSec_ / minorDivisionsIntervalsSec_) {
                leftOffsetFromFirstMajor = i;
            }
        }

        double firstMinorDivisionSec = firstMajorDivisionSec - leftOffsetFromFirstMajor * minorDivisionsIntervalsSec_;
        firstMinorDivision = ((firstMinorDivisionSec - start) / range) * soundwaveBounds_.getWidth();
        spaceBtwMinorDivisions = (minorDivisionsIntervalsSec_ / range) * soundwaveBounds_.getWidth();
    }

    foreachVerticalBar(soundwaveBounds_, firstMajorDivision, spaceBtwMajorDivisions, [this](float x){

        TimeStamp timeStamp;
        const int totalSeconds = juce::roundToInt(xToTime(x));
        timeStamp.text = juce::String(totalSeconds / 60) + ":"
                       + juce::String(totalSeconds % 60).paddedLeft('0', 2);
        timeStamp.bounds = {x - 20, soundwaveBounds_.getY() - 12, 40, 10};

        timeStamps_.push_back(timeStamp);

        trianglesPath_.addTriangle({x - triangleSize_, soundwaveBounds_.getY()},                    // left-top
                                   {x,                soundwaveBounds_.getY() + triangleSize_},   // center
                                   {x + triangleSize_, soundwaveBounds_.getY()});                 // right-top

        majorBarsPath_.addRectangle(juce::Rectangle<float>(thickLineThickness_, soundwaveBounds_.getHeight())
                                    .withCentre({x, soundwaveBounds_.getCentreY()}));

    });

    foreachVerticalBar(soundwaveBounds_, firstMinorDivision, spaceBtwMinorDivisions, [this](float x){
        minorBarsPath_.addRectangle(juce::Rectangle<float>(0.5f, soundwaveBounds_.getHeight())
                                    .withCentre({x, soundwaveBounds_.getCentreY()}));
    });

    // Keep the playback marker aligned with the (possibly changed) view mapping
    recreatePlayPositionMarker();
}

void SoundWavePath::mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel) {
    if ((wheel.deltaY > 0 && secRange_.getLength() <= 10.0f) ||
        (wheel.deltaY < 0 && secRange_.getLength() >= thumbnail_.getTotalLength()) ||
        (wheel.deltaY == 0)) {
            return;
        }

    const float modifier = (wheel.deltaY > 0 ? 1 : -1);
    const float mousePositionSec = (float)xToTime(event.position.getX());
    const float zoomStep = 0.1f;

    float newRange = secRange_.getLength() * (1 - modifier * zoomStep);
    newRange = juce::jlimit(10.0f, (float)thumbnail_.getTotalLength(), newRange);

    float newStart = secRange_.getStart() + modifier * (mousePositionSec - secRange_.getStart()) * zoomStep;
    float newEnd = secRange_.getEnd() - modifier * (secRange_.getEnd() - mousePositionSec) * zoomStep;
    
    if (newStart < 0) {
        newStart = 0;
        newEnd = juce::jmin(newRange, (float)(thumbnail_.getTotalLength()));
    } else if (newEnd > thumbnail_.getTotalLength()) {
        newEnd = thumbnail_.getTotalLength();
        newStart = juce::jmax(0.0f, newEnd - newRange);
    }

    secRange_.setStart(newStart);
    secRange_.setEnd(newEnd);

    onZoom(secRange_);
}

void SoundWavePath::setCurrentTime(double timeInSeconds)
{
    if (std::abs(currentTime_ - timeInSeconds) < 0.001)
        return; // Avoid unnecessary repaints

    currentTime_ = timeInSeconds;

    recreatePlayPositionMarker();
    repaint();
}

void SoundWavePath::recreatePlayPositionMarker()
{
    playPositionMarkerPath_.clear();

    // Nothing to draw if the current time is outside the visible range
    if (currentTime_ < secRange_.getStart() || currentTime_ > secRange_.getEnd())
        return;

    float xPos = juce::roundToInt(timeToX(currentTime_)); // whole numbers give a consistent view of the line accross positions

    // Draw vertical line
    playPositionMarkerPath_.addRectangle(juce::Rectangle<float>(thickLineThickness_, soundwaveBounds_.getHeight())
                                        .withCentre({xPos, soundwaveBounds_.getCentreY()}));
    
    // Draw triangle at the top of the vertical line (same style as major divisions)
    float triangleTop = soundwaveBounds_.getY();
    playPositionMarkerPath_.addTriangle(
                                       xPos, triangleTop + triangleSize_,
                                       xPos - triangleSize_, triangleTop,
                                       xPos + triangleSize_, triangleTop
                                       );
}

float SoundWavePath::timeToX(double sec) const
{
    if (secRange_.getLength() == 0) {
        return soundwaveBounds_.getX();
    }

    return soundwaveBounds_.getX()
         + (float)((sec - secRange_.getStart()) / secRange_.getLength()) * soundwaveBounds_.getWidth();
}

double SoundWavePath::xToTime(float x) const
{
    if (secRange_.getLength() == 0) {
        return 0.0;
    }

    return secRange_.getStart()
         + (x - soundwaveBounds_.getX()) / soundwaveBounds_.getWidth() * secRange_.getLength();
}

void SoundWavePath::renderWaveformToCache() { 
    auto bounds = soundwaveBounds_.getSmallestIntegerContainer();

    waveformCache_ = juce::Image(juce::Image::ARGB, bounds.getWidth(), bounds.getHeight(), true);

    if (thumbnail_.getNumChannels() != 0) {
        juce::Graphics gCache(waveformCache_);
        gCache.setColour(linesColour_);
        gCache.setOpacity(0.25f);
        thumbnail_.drawChannel (gCache, {0, 0, bounds.getWidth(), bounds.getHeight()},
                                secRange_.getStart(),   // start time
                                secRange_.getEnd(),     // end time
                                channel_,
                                0.6f);                  // vertical zoom
    }

    waveformCacheDirty_ = false;
}
