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
    linesColour = lines;
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
    g.setColour (linesColour);

    g.drawHorizontalLine(drawingBounds.getCentreY(), drawingBounds.getX(), drawingBounds.getRight());

    foreachVerticalBar(drawingBounds, numMajorDivisions, [this, &g](float x){
        g.fillRect(juce::Rectangle<float>(thickLineThickness, drawingBounds.getHeight()).withCentre({x, drawingBounds.getCentreY()}));
        DBG("line x : " << x );
    });

    g.fillPath(barTrianglesPath);

    g.setOpacity(0.15f);

    foreachVerticalBar(drawingBounds, numMinorDivisions, [this, &g](float x){
        g.fillRect(juce::Rectangle<float>(thickLineThickness, drawingBounds.getHeight()).withCentre({x, drawingBounds.getCentreY()}));
    });

}

void SoundWavePath::resized()
{
    barTrianglesPath.clear();
    
    drawingBounds = getLocalBounds().toFloat().reduced(thickLineThickness / 2.0f, 0);


    float triangleSize = 8.0f;
    foreachVerticalBar(drawingBounds, numMajorDivisions, [this, triangleSize](float x){
        barTrianglesPath.addTriangle({x - triangleSize, drawingBounds.getY()},                  // left-top
                                     {x,                drawingBounds.getY() + triangleSize},   // center
                                     {x + triangleSize, drawingBounds.getY()});                 // right-top
    });

}
