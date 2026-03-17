/*
  ==============================================================================

    MainComponent.cpp
    Created: 16 Mar 2026 8:24:34pm
    Author:  Yuliia Ruda

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainComponent.h"

juce::String text = "I see it all perfectly; there are two possible situations - one can either do this or that. My honest opinion and my friendly advice is this:  do it or do not do it - you will regret both.";

//==============================================================================
MainComponent::MainComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setSize(900, 600);
}

MainComponent::~MainComponent()
{
}

juce::FontOptions MainComponent::getHeadingFont() {
    return juce::FontOptions("Sans Serif", fontSize, juce::Font::bold);
}

juce::FontOptions MainComponent::getTextFont() {
    return juce::FontOptions("Sans Serif", 20, juce::Font::plain);
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::lightsalmon);

    g.setColour (juce::Colours::coral);
    g.drawRect (getLocalBounds(), 10);   // draw an outline around the component

    g.setColour (juce::Colours::midnightblue);
    g.setFont (getHeadingFont());
    auto headerBounds = juce::Rectangle<int>(20, 20, getWidth() - 40, 200 - 20);
    g.drawText (juce::String::fromUTF8("Søren Kierkegaard"), headerBounds,
                juce::Justification::centredLeft, true /* to show ... or just cut if not within bounds */);
    
    g.setFont(getTextFont());
    g.drawMultiLineText(text, 60, 200 + 20, getWidth() - 20 - 60); // the second number indicates the bottom of the first
                                                           // row of letters
    
    g.setColour(juce::Colours::lightgrey);
    auto houseRect = juce::Rectangle<int>(600, 370, 200, 170);
    
//    g.setGradientFill(juce::ColourGradient(juce::Colours::lightgrey, houseRect.toFloat().getX(), houseRect.toFloat().getY(), juce::Colours::grey, houseRect.toFloat().getRight(), houseRect.toFloat().getBottom(), false));
    g.fillRect(houseRect);
    
    juce::Path myPath;
     
    myPath.startNewSubPath(     40, 200);
    myPath.quadraticTo (20, 220, 40, 240);
    myPath.quadraticTo (20, 260, 40, 280);
    myPath.quadraticTo (20,    300, 40, 320);
     
    // add an ellipse as well, which will form a second sub-path within the path
    myPath.addEllipse (74.0f, 45.0f, 60.0f, 30.0f);
     
    // double the width of the whole thing
//    myPath.applyTransform (juce::AffineTransform::scale (2.0f, 1.0f));
     
    // and draw it to a graphics context with a 5-pixel thick outline
    g.strokePath (myPath, juce::PathStrokeType (5.0f));
    
}

void MainComponent::resized()
{
    DBG(getWidth());
    fontSize = getWidth() / 10.f;
}
