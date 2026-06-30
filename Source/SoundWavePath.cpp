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
    calculateDivisions();
}

void SoundWavePath::changeListenerCallback(juce::ChangeBroadcaster *source) {
    if (source == &thumbnail_) {
        repaint();
    }

    DBG("change");
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

    const float range = secRange_.getLength();
    const int fullMinutesRange = (int)(range / 60);

    // TODO: what is the maximum?
    if (fullMinutesRange > 7) {
        majorDivisionsIntervalsSec_ = (fullMinutesRange / 4) * 60;
    } else if (fullMinutesRange > 2) {
        majorDivisionsIntervalsSec_ = 60;
    } else {
        majorDivisionsIntervalsSec_ = range / 5;
    }

    calculateDivisions();
    repaint(); // TODO: not sure about it!
}

void SoundWavePath::calculateDivisions() {
    float start = secRange_.getStart();
    float end = secRange_.getEnd();
    float range = secRange_.getLength();

    trianglesPath_.clear();
    majorBarsPath_.clear();
    minorBarsPath_.clear();
    timeStamps_.clear();
    
    float firstMajorDivision = 0;
    float firstMinorDivision = 0;
    float spaceBtwMajorDivisions = soundwaveBounds_.getWidth() / 5.0f;
    float spaceBtwMinorDivisions = (soundwaveBounds_.getWidth() / 5.0f) / 6.0f;
    if (end > 0) {
        int firstMajorDivisionSec = 0;
        if ((int)std::ceil(secRange_.getStart()) % majorDivisionsIntervalsSec_ == 0) {
            firstMajorDivisionSec = (int)std::ceil(start) / majorDivisionsIntervalsSec_;
        } else {
            firstMajorDivisionSec = (int)std::ceil(start) / majorDivisionsIntervalsSec_ + majorDivisionsIntervalsSec_;
        }
        firstMajorDivision = (((float)firstMajorDivisionSec - start)/ range ) * soundwaveBounds_.getWidth();
        spaceBtwMajorDivisions = ((float)majorDivisionsIntervalsSec_ / range ) * soundwaveBounds_.getWidth();

        float minorDivisionsIntervalSec = (float)majorDivisionsIntervalsSec_ / 6.0f;
        int leftOffsetFromFirstMajor = 0;
        for (int i = 0; i < 6; i++) {
            if ((firstMajorDivisionSec - i * minorDivisionsIntervalSec) < start) {
                leftOffsetFromFirstMajor = i;
            }
        }
        float firstMinorDivisionSec = firstMajorDivisionSec - leftOffsetFromFirstMajor * minorDivisionsIntervalSec;
        firstMinorDivision = ((firstMinorDivisionSec - start) / range) * soundwaveBounds_.getWidth();
        spaceBtwMinorDivisions = (minorDivisionsIntervalSec / range) * soundwaveBounds_.getWidth();
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
        (wheel.deltaY < 0 && secRange_.getLength() >= thumbnail_.getTotalLength())) {
            return;
        }

    float modifier = (wheel.deltaY > 0 ? 1 : -1);

    float positionSec = (event.position.getX() /* not sure which X is gonna be here*/ / soundwaveBounds_.getWidth()) * (secRange_.getLength());

    secRange_.setStart(secRange_.getStart() + modifier * (positionSec - secRange_.getStart()) * 0.1f);
    secRange_.setEnd(secRange_.getEnd() - modifier * (secRange_.getEnd() - positionSec) * 0.1f);
    
    resetTimeBounds(secRange_.getStart(), secRange_.getEnd());
}
