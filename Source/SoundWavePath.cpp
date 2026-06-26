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

    g.drawHorizontalLine(drawingBounds_.getCentreY(), drawingBounds_.getX(), drawingBounds_.getRight());
    
    g.setOpacity(0.15f);
    g.fillPath(minorBarsPath_);

    g.setOpacity(1.0f);
    g.fillPath(majorBarsPath_);
    g.fillPath(trianglesPath_);

    // Audio waveform
    g.setOpacity(0.15f);
    if (thumbnail_.getNumChannels() != 0) {
        thumbnail_.drawChannel (g, drawingBounds_.toNearestInt(),
                                 0.0,                           // start time
                                 thumbnail_.getTotalLength(),   // end time
                                 channel_,
                                 0.5f);                         // vertical zoom
    }

}

void SoundWavePath::resized()
{
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

// Can be used when loading a new song, or scaling
void SoundWavePath::setNewSongLength() {
    resetTimeBounds(0.0f, thumbnail_.getTotalLength());
}

// Reset time bounds
// Reset scale pixels
// I'm not sure about repaint() at the end
void SoundWavePath::resetTimeBounds(const float minSec, const float maxSec) {
    minSec_ = minSec;
    maxSec_ = maxSec;

    const float range = maxSec_ - minSec_;
    const int fullMinutesRange = (int)(range / 60);

    if (fullMinutesRange > 7) {
        majorDivisionsIntervalsSec_ = (fullMinutesRange / 4) * 60;
    } else if (fullMinutesRange > 2) {
        majorDivisionsIntervalsSec_ = 60;
    } else {
        majorDivisionsIntervalsSec_ = range / 5;
    }

    calculateDivisions();
    repaint();
}

void SoundWavePath::calculateDivisions() {
    drawingBounds_ = getLocalBounds().toFloat().reduced(thickLineThickness_ / 2.0f, 0);

    trianglesPath_.clear();
    majorBarsPath_.clear();
    minorBarsPath_.clear();
    
    float firstMajorDivision = 0;
    float firstMinorDivision = 0;
    float spaceBtwMajorDivisions = drawingBounds_.getWidth() / 5.0f;
    float spaceBtwMinorDivisions = (drawingBounds_.getWidth() / 5.0f) / 6.0f;
    if (maxSec_ > 0) {
        int firstMajorDivisionSec = 0;
        if ((int)std::ceil(minSec_) % majorDivisionsIntervalsSec_ == 0) {
            firstMajorDivisionSec = (int)std::ceil(minSec_) / majorDivisionsIntervalsSec_;
        } else {
            firstMajorDivisionSec = (int)std::ceil(minSec_) / majorDivisionsIntervalsSec_ + majorDivisionsIntervalsSec_;
        }
        firstMajorDivision = ((float)firstMajorDivisionSec - minSec_)/(maxSec_ - minSec_) * drawingBounds_.getWidth();
        spaceBtwMajorDivisions = ((float)majorDivisionsIntervalsSec_ / (maxSec_ - minSec_)) * drawingBounds_.getWidth();

        float minorDivisionsIntervalSec = (float)majorDivisionsIntervalsSec_ / 6.0f;
        int leftOffsetFromFirstMajor = 0;
        for (int i = 0; i < 6; i++) {
            if ((firstMajorDivisionSec - i * minorDivisionsIntervalSec) < minSec_) {
                leftOffsetFromFirstMajor = i;
            }
        }
        float firstMinorDivisionSec = firstMajorDivisionSec - leftOffsetFromFirstMajor * minorDivisionsIntervalSec;
        firstMinorDivision = (firstMinorDivisionSec - minSec_)/(maxSec_ - minSec_) * drawingBounds_.getWidth();
        spaceBtwMinorDivisions = (minorDivisionsIntervalSec / (maxSec_ - minSec_)) * drawingBounds_.getWidth();
    }

    foreachVerticalBar(drawingBounds_, firstMajorDivision, spaceBtwMajorDivisions, [this](float x){
        trianglesPath_.addTriangle({x - triangleSize_, drawingBounds_.getY()},                  // left-top
                                     {x,                drawingBounds_.getY() + triangleSize_},   // center
                                     {x + triangleSize_, drawingBounds_.getY()});                 // right-top
    });

    foreachVerticalBar(drawingBounds_, firstMajorDivision, spaceBtwMajorDivisions, [this](float x){
        majorBarsPath_.addRectangle(juce::Rectangle<float>(thickLineThickness_, drawingBounds_.getHeight()).withCentre({x, drawingBounds_.getCentreY()}));
    });

    foreachVerticalBar(drawingBounds_, firstMinorDivision, spaceBtwMinorDivisions, [this](float x){
        minorBarsPath_.addRectangle(juce::Rectangle<float>(thickLineThickness_, drawingBounds_.getHeight()).withCentre({x, drawingBounds_.getCentreY()}));
    });
}
