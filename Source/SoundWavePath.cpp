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

void SoundWavePath::paint (juce::Graphics& g)
{

    g.setColour (linesColour);
    
    float thickLineThickness = 1.0f;
    auto drawingBounds = getLocalBounds().toFloat().reduced(thickLineThickness / 2.0f);
    
    int numOfDivisions = 5;
    float widthOfDivision = drawingBounds.getWidth() / numOfDivisions;
    
    float triangleSize = 5.0f;
    
    juce::Path p;
    for (int div = 0; div <= numOfDivisions; ++div) {
        float x = drawingBounds.getX() + widthOfDivision * div;
        
        p.addTriangle({x - triangleSize, 0.0f}, {x, triangleSize}, {x + triangleSize, 0.0f});
        g.drawLine(x, 0.0f, x, drawingBounds.getHeight(), thickLineThickness);
    }
    g.fillPath(p);
    
    int numOfSmallDivisions = 30;
    float widthOfSmallDivision = drawingBounds.getWidth() / numOfSmallDivisions;
    for (int div = 0; div <= numOfSmallDivisions; ++div) {
        float x = drawingBounds.getX() + widthOfSmallDivision * div;
        g.drawLine(x, 0.0f, x, drawingBounds.getHeight(), 0.15f);
    }
    
    g.drawHorizontalLine(getHeight() / 2.0f, 0.0f, getWidth());
}

void SoundWavePath::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
