/*
  ==============================================================================

    YellowPlayer.cpp
    Created: 18 Mar 2026 8:21:57am
    Author:  Yuliia Ruda

  ==============================================================================
*/

#include <JuceHeader.h>
#include "YellowPlayer.h"

//============================== SHAPES ========================================

juce::Path plusShape(float size, float thickness) {
    juce::Path plus;

    // horisontal long rectangle
    plus.addRectangle(0, size / 2.0f - thickness / 2.0f, size, thickness);
    // vertical long rectangle
    plus.addRectangle(size / 2.0f - thickness / 2.0f, 0, thickness, size);
    
    return plus;
}

juce::Path pauseShape(float height, float width, float thickness) {
    juce::Path pause;

    // two long vertical rectangles
    pause.addRectangle(0, 0, thickness, height);
    pause.addRectangle(width - thickness, 0, thickness, height);
    
    return pause;
}

juce::Path playShape(float height, float width) {
    juce::Path play;

    // two long vertical rectangles
    play.addTriangle(0, 0, width, height / 2.0, 0, height);
    
    return play;
}

//==============================================================================
YellowPlayer::YellowPlayer()
    : pauseButton_("Pause", YellowPlayerColours::lime, YellowPlayerColours::lime.brighter(), YellowPlayerColours::lime)
    , playButton_("Play", YellowPlayerColours::lime, YellowPlayerColours::lime.brighter(), YellowPlayerColours::lime)
    , plusButton_("Add song", YellowPlayerColours::lime, YellowPlayerColours::lime.brighter(), YellowPlayerColours::lime)
{
    setAudioChannels (/* input channels */0, /*output channels */2);
    
    setSize(600, 600);
    setLookAndFeel(&lookAndFeel);

    // Just to force to recreate text box using the custom look and feel of the parent
    topSlider_.setLookAndFeel(&lookAndFeel);
    bottomSlider_.setLookAndFeel(&lookAndFeel);
    
    float buttonSize = 50.0f;
    float linesWidth = 10.0f;

    pauseButton_.setShape(pauseShape(buttonSize, 30.f, linesWidth), true, true, true);
    playButton_.setShape(playShape(buttonSize, buttonSize * 0.75f), true, true, true);
    plusButton_.setShape(plusShape(buttonSize, linesWidth), true, true, true);

    addAndMakeVisible(topSlider_);
    addAndMakeVisible(bottomSlider_);
    addAndMakeVisible(topSoundWave_);

    addAndMakeVisible(pauseButton_);
    addAndMakeVisible(playButton_);
    addAndMakeVisible(plusButton_);
}

YellowPlayer::~YellowPlayer()
{
    setLookAndFeel(nullptr);
    shutdownAudio();
}

void YellowPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

}

void YellowPlayer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
}

void YellowPlayer::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

// ============================ GUI ============================

void YellowPlayer::paint (juce::Graphics& g)
{
    // Background
    g.fillAll (YellowPlayerColours::black);

    // Central component
    g.setColour (YellowPlayerColours::darkGrey);

    auto centralArea = getLocalBounds().toFloat().reduced(10.f);
    auto centralAreaCopy = centralArea;
    auto controlsArea = centralAreaCopy.removeFromBottom(controlsAreaHeight_);    // bottom 10% in original size
    auto underPlayersArea = centralAreaCopy.removeFromBottom(60.0f);
    auto playerArea = centralAreaCopy;                              // all area - 60 - 60
    
    g.fillRoundedRectangle(playerArea, 20.0f);

    // Player components
    g.setColour(YellowPlayerColours::black.brighter());
    auto playerAreaCopy = playerArea;
    auto topPlayerArea = playerAreaCopy.removeFromTop(playerArea.getHeight() / 2).reduced(10, 7.5f).translated(0, 2.5f);
    auto bottomPlayerArea = playerAreaCopy.removeFromTop(playerArea.getHeight() / 2).reduced(10, 7.5f).translated(0, -2.5f);
    
    g.fillRoundedRectangle(topPlayerArea, 20.0f - 5.0f);
    g.fillRoundedRectangle(bottomPlayerArea, 20.0f - 5.0f);

    topSlider_.setBounds(topPlayerArea.withHeight(20).withBottomY(topPlayerArea.getBottom() - 15).reduced(10, 0).toNearestInt());
    bottomSlider_.setBounds(bottomPlayerArea.withHeight(20).withBottomY(bottomPlayerArea.getBottom() - 15).reduced(10, 0).toNearestInt());

    topSoundWave_.setBounds(topPlayerArea.reduced(10, 40).translated(0, -5).toNearestInt());

    // Controls
    pauseButton_.setBounds(controlsArea.withLeft(controlsArea.getCentreX()).withRight(controlsArea.getCentreX() + controlsAreaHeight_).toNearestInt());
    playButton_.setBounds(controlsArea.withLeft(controlsArea.getCentreX() + controlsAreaHeight_).withRight(controlsArea.getCentreX() + controlsAreaHeight_ * 2).toNearestInt());
    plusButton_.setBounds(controlsArea.withLeft(controlsArea.getRight() - controlsAreaHeight_).toNearestInt());
    
    // Text
    auto clockArea = controlsArea.removeFromLeft(controlsArea.getWidth() / 2.0f);
    auto clockAreaDrawingBounds = clockArea.reduced(2.0f);
    g.setColour(lookAndFeel.findColour(juce::Label::textColourId));
    
    juce::Time currentTime = juce::Time::getCurrentTime();
    juce::String timeString = juce::String (juce::String(currentTime.getHours()).paddedLeft('0', 2) + ":" + (juce::String(currentTime.getMinutes())).paddedLeft('0', 2));
    auto clockFont = juce::FontOptions ("Helvetica", 60.0f * helveticaMultiplier, juce::Font::plain);
    g.setFont(clockFont);
    g.drawSingleLineText (timeString, clockAreaDrawingBounds.getX(), clockAreaDrawingBounds.getBottom());
}

void YellowPlayer::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
