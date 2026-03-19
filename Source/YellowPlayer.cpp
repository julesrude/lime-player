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
    auto controlsArea = centralArea.withTop(centralArea.getBottom() - 60.0f); // bottom 10%
    auto playerArea = centralArea.withTrimmedBottom(controlsArea.getHeight() + 60.f);
//    auto playerArea = centralArea.withHeight(centralArea.getHeight() * 0.8); // top 80%

    
    DBG("player area height BEFORE top player are calc:" << playerArea.getHeight());
    DBG(centralArea.getHeight());
    g.fillRoundedRectangle(playerArea, 20.0f);

    // Text
    g.setColour(lookAndFeel.findColour(juce::Label::textColourId));

    juce::Time currentTime = juce::Time::getCurrentTime();
    juce::String timeString = juce::String(juce::String(currentTime.getHours()).paddedLeft('0', 2) + ":" + (juce::String(currentTime.getMinutes())).paddedLeft('0', 2));
    g.setFont (juce::FontOptions("Helvetica", 60, juce::Font::plain));
    g.drawText(timeString, controlsArea, juce::Justification::bottomLeft);

    // Player components
    g.setColour(lookAndFeel.blackColor.brighter());
    auto topPlayerArea = playerArea.withTrimmedTop(10.0f).withHeight((playerArea.getHeight() - 10.0f * 3) / 2).reduced(10, 0);
    DBG("player area height after top player are calc:" << playerArea.getHeight());
    auto bottomPlayerArea = playerArea.withTrimmedTop(10.0f + topPlayerArea.getHeight() + 10.0f).withHeight((playerArea.getHeight() - 10.0f * 3) / 2).reduced(10, 0);
    
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
