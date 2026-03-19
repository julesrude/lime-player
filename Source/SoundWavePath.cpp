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
    g.drawHorizontalLine(drawingBounds.getHeight() / 2.0f, 0.0f, drawingBounds.getWidth());

    int numMajorDivisions = 5;
    float widthMajorDivision = drawingBounds.getWidth() / numMajorDivisions;
    
    float triangleSize = 5.0f;
    
    juce::Path p;
    for (int div = 0; div <= numMajorDivisions; ++div) {
        float x = drawingBounds.getX() + widthMajorDivision * div;
        
        p.addTriangle({x - triangleSize, 0.0f},
                      {x,                triangleSize},
                      {x + triangleSize, 0.0f});
        g.drawLine(x, 0.0f, x, drawingBounds.getHeight(), thickLineThickness);
    }
    g.fillPath(p);
    
    g.setOpacity(0.15f);
    int numMinorDivisions = 30;
    float widthMinorDivision = drawingBounds.getWidth() / numMinorDivisions;
    for (int div = 0; div <= numMinorDivisions; ++div) {
        float x = drawingBounds.getX() + widthMinorDivision * div;
        g.drawLine(x, 0.0f, x, drawingBounds.getHeight());
    }
}

void SoundWavePath::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
