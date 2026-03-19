/*
  ==============================================================================

    YellowPlayer.cpp
    Created: 18 Mar 2026 8:21:57am
    Author:  Yuliia Ruda

  ==============================================================================
*/

#include <JuceHeader.h>
#include "YellowPlayer.h"

//==============================================================================
YellowPlayer::YellowPlayer()
{
    setSize(600, 600);
    setLookAndFeel(&lookAndFeel);

    // To force to recreate text box using the custom look and feel of the parent
    topSlider_.setLookAndFeel(&lookAndFeel);
    bottomSlider_.setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(topSlider_);
    addAndMakeVisible(bottomSlider_);
}

YellowPlayer::~YellowPlayer()
{
    setLookAndFeel(nullptr);
}

void YellowPlayer::paint (juce::Graphics& g)
{
    // Background
    g.fillAll (lookAndFeel.blackColor);

    // Central component
    g.setColour (lookAndFeel.darkGreyColor);

    auto centralArea = getLocalBounds().toFloat().reduced(10.f);
    auto centralAreaCopy = centralArea;
    auto controlsArea = centralAreaCopy.removeFromBottom(60.0f);    // bottom 10% in original size
    auto underPlayersArea = centralAreaCopy.removeFromBottom(60.0f);
    auto playerArea = centralAreaCopy;                              // all area - 60 - 60
    
    g.fillRoundedRectangle(playerArea, 20.0f);

    // Text
    g.setColour(lookAndFeel.findColour(juce::Label::textColourId));

    juce::Time currentTime = juce::Time::getCurrentTime();
    juce::String timeString = juce::String(juce::String(currentTime.getHours()).paddedLeft('0', 2) + ":" + (juce::String(currentTime.getMinutes())).paddedLeft('0', 2));
    g.setFont (juce::FontOptions("Helvetica", 60, juce::Font::plain));
    g.drawText(timeString, controlsArea, juce::Justification::bottomLeft);

    // Player components
    g.setColour(lookAndFeel.blackColor.brighter());
    auto playerAreaCopy = playerArea;
    auto topPlayerArea = playerAreaCopy.removeFromTop(playerArea.getHeight() / 2).reduced(10, 7.5f).translated(0, 2.5f);
    auto bottomPlayerArea = playerAreaCopy.removeFromTop(playerArea.getHeight() / 2).reduced(10, 7.5f).translated(0, -2.5f);
    
    g.fillRoundedRectangle(topPlayerArea, 20.0f - 5.0f);
    g.fillRoundedRectangle(bottomPlayerArea, 20.0f - 5.0f);
    
    // ----
    //
    topSlider_.setBounds(topPlayerArea.withHeight(20).withBottomY(topPlayerArea.getBottom() - 15).reduced(10, 0).toNearestInt());
    bottomSlider_.setBounds(bottomPlayerArea.withHeight(20).withBottomY(bottomPlayerArea.getBottom() - 15).reduced(10, 0).toNearestInt());
    
}

void YellowPlayer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
