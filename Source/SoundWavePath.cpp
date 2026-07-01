/*
  ==============================================================================

    SoundWavePath.cpp
    Created: 19 Mar 2026 1:44:51pm
    Author:  Yuliia Ruda

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SoundWavePath.h"

//==============================================================================
SoundWavePath::SoundWavePath(juce::AudioThumbnail &thumbnail, const int channel)
    : thumbnail_(thumbnail),
      channel_(channel)
{
    thumbnail_.addChangeListener(this);
}

SoundWavePath::~SoundWavePath()
{
}

namespace {
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
    g.setOpacity(0.15f);
    if (thumbnail_.getNumChannels() != 0) {
        thumbnail_.drawChannel (g, soundwaveBounds_.toNearestInt(),
                                secRange_.getStart(),               // start time
                                secRange_.getEnd(),                 // end time
                                channel_,
                                0.7f);                              // vertical zoom
    }

    // Time stamps on major bars
    g.setOpacity(0.5f);
    g.setFont(YellowPlayerLookAndFeel::getHelveticaForHeight(8));
    for (auto &timePoint : timeStamps_) {
        g.drawText(timePoint.text, timePoint.bounds, juce::Justification::centred);
    }

}

void SoundWavePath::resized()
{
    soundwaveBounds_ = getLocalBounds().toFloat().reduced(thickLineThickness_ / 2.0f, 10.0f).translated(0, 10.0f);
    calculateResizedDivisions();
}

void SoundWavePath::changeListenerCallback(juce::ChangeBroadcaster *source) {
    if (source == &thumbnail_) {
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

    std::pair<juce::RelativeTime, juce::RelativeTime> intervals = majMinIntervalFromRange(secRange_.getLength());

    majorDivisionsIntervalsSec_ = intervals.first.inSeconds();
    minorDivisionsIntervalsSec_ = intervals.second.inSeconds();

    calculateResizedDivisions();
    repaint(); // TODO: not sure about it!
}

void SoundWavePath::calculateResizedDivisions() {
    const float start = secRange_.getStart();
    const float end = secRange_.getEnd();
    const float range = secRange_.getLength();
    
    trianglesPath_.clear();
    majorBarsPath_.clear();
    minorBarsPath_.clear();
    timeStamps_.clear();
    
    float firstMajorDivision = 0;
    float firstMinorDivision = 0;
    
    float spaceBtwMajorDivisions = soundwaveBounds_.getWidth() / 5.0f;
    float spaceBtwMinorDivisions = (soundwaveBounds_.getWidth() / 5.0f) / 6.0f;

    if (end > 0) {
        spaceBtwMajorDivisions = majorDivisionsIntervalsSec_ / range * soundwaveBounds_.getWidth();
        spaceBtwMinorDivisions = minorDivisionsIntervalsSec_ / range * soundwaveBounds_.getWidth();
        
        float firstMajorDivisionSec = 0;
        if (secRange_.contains((int)(start / majorDivisionsIntervalsSec_))) {
            firstMajorDivisionSec = (int)(start / majorDivisionsIntervalsSec_) * majorDivisionsIntervalsSec_;
        } else {
            firstMajorDivisionSec = (int)(start / majorDivisionsIntervalsSec_) * majorDivisionsIntervalsSec_ + majorDivisionsIntervalsSec_;
        }

        firstMajorDivision = ((firstMajorDivisionSec - start) / range ) * soundwaveBounds_.getWidth();
        spaceBtwMajorDivisions = (majorDivisionsIntervalsSec_ / range ) * soundwaveBounds_.getWidth();

        int leftOffsetFromFirstMajor = 0;
        for (int i = 0; i < majorDivisionsIntervalsSec_ / minorDivisionsIntervalsSec_; i++) {
            float candidate = firstMajorDivisionSec - i * minorDivisionsIntervalsSec_;
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

        float firstMinorDivisionSec = firstMajorDivisionSec - leftOffsetFromFirstMajor * minorDivisionsIntervalsSec_;
        firstMinorDivision = ((firstMinorDivisionSec - start) / range) * soundwaveBounds_.getWidth();
        spaceBtwMinorDivisions = (minorDivisionsIntervalsSec_ / range) * soundwaveBounds_.getWidth();
    }

    foreachVerticalBar(soundwaveBounds_, firstMajorDivision, spaceBtwMajorDivisions, [this, start, range](float x){

        TimeStamp timeStamp;
        juce::RelativeTime timePoint(start + (x / soundwaveBounds_.getWidth()) * range);
        timeStamp.text = juce::String (juce::String((int)timePoint.inMinutes()) + ":" + (juce::String((int)timePoint.inSeconds() % 60)).paddedLeft('0', 2));
        timeStamp.bounds = {x - 20, soundwaveBounds_.getY() - 12, 40, 10};

        timeStamps_.push_back(timeStamp);

        trianglesPath_.addTriangle({x - triangleSize_, soundwaveBounds_.getY()},                    // left-top
                                   {x,                soundwaveBounds_.getY() + triangleSize_},   // center
                                   {x + triangleSize_, soundwaveBounds_.getY()});                 // right-top

        majorBarsPath_.addRectangle(juce::Rectangle<float>(thickLineThickness_, soundwaveBounds_.getHeight()).withCentre({x, soundwaveBounds_.getCentreY()}));

    });

    foreachVerticalBar(soundwaveBounds_, firstMinorDivision, spaceBtwMinorDivisions, [this](float x){
        minorBarsPath_.addRectangle(juce::Rectangle<float>(thickLineThickness_, soundwaveBounds_.getHeight()).withCentre({x, soundwaveBounds_.getCentreY()}));
    });
}

void SoundWavePath::mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel) {
    if ((wheel.deltaY > 0 && secRange_.getLength() <= 10.0f) ||
        (wheel.deltaY < 0 && secRange_.getLength() >= thumbnail_.getTotalLength()) ||
        (wheel.deltaY == 0)) {
            return;
        }

    const float modifier = (wheel.deltaY > 0 ? 1 : -1);
    const float mousePositionSec = secRange_.getStart() + event.position.getX() / soundwaveBounds_.getWidth() * secRange_.getLength();
    const float zoomStep = 0.1;

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

    resetTimeBounds(secRange_.getStart(), secRange_.getEnd());
}
