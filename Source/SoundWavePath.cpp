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
SoundWavePath::SoundWavePath(juce::Colour lines)
{
    linesColour_ = lines;
}

SoundWavePath::~SoundWavePath()
{
}

namespace {
template <typename Action>
void foreachVerticalBar(const juce::Rectangle<float> drawingBounds,
                        const int numDivisions,
                        Action&& action) {
    
    float spaceBtwBars = drawingBounds.getWidth() / numDivisions;
    
    // Number of bars will be 1 more than number of divisions
    for (int bar = 0; bar <= numDivisions; ++bar) {
        float x = drawingBounds.getX() + spaceBtwBars * bar;
        
        action(x);
    }
}
}

void SoundWavePath::paint (juce::Graphics& g)
{
    g.setColour (linesColour_);

    g.drawHorizontalLine(drawingBounds_.getCentreY(), drawingBounds_.getX(), drawingBounds_.getRight());

    foreachVerticalBar(drawingBounds_, numMajorDivisions_, [this, &g](float x){
        g.fillRect(juce::Rectangle<float>(thickLineThickness_, drawingBounds_.getHeight()).withCentre({x, drawingBounds_.getCentreY()}));
    });

    g.fillPath(barTrianglesPath_);

    g.setOpacity(0.15f);

    foreachVerticalBar(drawingBounds_, numMinorDivisions_, [this, &g](float x){
        g.fillRect(juce::Rectangle<float>(thickLineThickness_, drawingBounds_.getHeight()).withCentre({x, drawingBounds_.getCentreY()}));
    });

}

void SoundWavePath::resized()
{
    barTrianglesPath_.clear();
    
    drawingBounds_ = getLocalBounds().toFloat().reduced(thickLineThickness_ / 2.0f, 0);

    foreachVerticalBar(drawingBounds_, numMajorDivisions_, [this](float x){
        barTrianglesPath_.addTriangle({x - triangleSize_, drawingBounds_.getY()},                  // left-top
                                     {x,                drawingBounds_.getY() + triangleSize_},   // center
                                     {x + triangleSize_, drawingBounds_.getY()});                 // right-top
    });

}
