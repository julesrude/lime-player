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

//============================== LISTENER ========================================

void YellowPlayer::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &transportSource_)
    {
        if (transportSource_.isPlaying()) {
            state_ = TransportState::Playing;
        } else {
            state_ = TransportState::Paused;
        }
    }
}

//==============================================================================
YellowPlayer::YellowPlayer()
    : pauseButton_("Pause", YellowPlayerColours::lime, YellowPlayerColours::lime.brighter(), YellowPlayerColours::lime)
    , playButton_("Play", YellowPlayerColours::lime, YellowPlayerColours::lime.brighter(), YellowPlayerColours::lime)
    , plusButton_("Add song", YellowPlayerColours::lime, YellowPlayerColours::lime.brighter(), YellowPlayerColours::lime)
    , state_(TransportState::Paused)
{
    setAudioChannels (/* input channels */0, /*output channels */2);
    formatManager_.registerBasicFormats();
    transportSource_.addChangeListener(this);
    
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

    DBG("Add and make visible");
    addAndMakeVisible(pauseButton_);
    addAndMakeVisible(playButton_);
    addAndMakeVisible(plusButton_);

    plusButton_.onClick = [this]{ plusButtonClicked(); };
    playButton_.onClick = [this]{ playButtonClicked(); };
    pauseButton_.onClick = [this]{ pauseButtonClicked(); };

    playButton_.setEnabled(false);
    pauseButton_.setEnabled(false);

    setSize(600, 600);
}

YellowPlayer::~YellowPlayer()
{
    setLookAndFeel(nullptr);
    shutdownAudio();
}

void YellowPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource_.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void YellowPlayer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!readerSource_)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    transportSource_.getNextAudioBlock(bufferToFill);
}

void YellowPlayer::releaseResources()
{
    transportSource_.releaseResources();
}

// ============================ GUI ============================

void YellowPlayer::paint (juce::Graphics& g)
{
    static int paint_counter = 0;
    DBG("Repaint #" << paint_counter++);
    
    // Background
    g.fillAll (YellowPlayerColours::black);

    // Central component
    g.setColour (YellowPlayerColours::darkGrey);
    g.fillRoundedRectangle(playerArea_, 20.0f);

    // Player components
    g.setColour(YellowPlayerColours::black.brighter());
    g.fillRoundedRectangle(topPlayerArea_, 20.0f - 5.0f);
    g.fillRoundedRectangle(bottomPlayerArea_, 20.0f - 5.0f);

    // Time
    auto clockAreaDrawingBounds = timeArea_.reduced(2.0f);
    g.setColour(lookAndFeel.findColour(juce::Label::textColourId));

    juce::RelativeTime currentTime(transportSource_.getCurrentPosition());
    juce::String timeString = juce::String (juce::String((int)currentTime.inMinutes()).paddedLeft('0', 2) + ":" + (juce::String((int)currentTime.inSeconds() % 60)).paddedLeft('0', 2));
    auto clockFont = juce::FontOptions ("Helvetica", controlsAreaHeight_ * helveticaMultiplier, juce::Font::plain);
    g.setFont(clockFont);
    g.drawSingleLineText (timeString, clockAreaDrawingBounds.getX(), clockAreaDrawingBounds.getBottom());
}

void YellowPlayer::resized()
{
    static int resize_counter = 0;
    DBG("Resize #" << resize_counter++);

    // Big areas
    centralArea_ = getLocalBounds().toFloat().reduced(10.f);

    // [ the order of these is important
    auto centralAreaCopy = centralArea_;
    controlsArea_ = centralAreaCopy.removeFromBottom(controlsAreaHeight_);  // bottom 10% in original size
    underPlayersArea_ = centralAreaCopy.removeFromBottom(controlsAreaHeight_);
    playerArea_ = centralAreaCopy;  // all area - controlsArea - underPlayersArea

    // Player area components
    topPlayerArea_ = centralAreaCopy.removeFromTop(playerArea_.getHeight() / 2).reduced(10, 7.5f).translated(0, 2.5f);
    bottomPlayerArea_ = centralAreaCopy.removeFromTop(playerArea_.getHeight() / 2).reduced(10, 7.5f).translated(0, -2.5f);
    // closing the area cutting sequence ]

    topSoundWave_.setBounds(topPlayerArea_.reduced(10, 40).translated(0, -5).toNearestInt());

    topSlider_.setBounds(topPlayerArea_.withHeight(20).withBottomY(topPlayerArea_.getBottom() - 15).reduced(10, 0).toNearestInt());
    bottomSlider_.setBounds(bottomPlayerArea_.withHeight(20).withBottomY(bottomPlayerArea_.getBottom() - 15).reduced(10, 0).toNearestInt());

    // Controls area components
    auto controlsAreaCopy = controlsArea_;
    timeArea_ = controlsAreaCopy.removeFromLeft(controlsArea_.getWidth() / 2.0f);

    DBG(controlsAreaHeight_);
    DBG("Controls area top : " << controlsArea_.getY());
    DBG("Controls area bottom : " << controlsArea_.getBottom());
    DBG("Window bottom : " << getLocalBounds().getBottom());
    pauseButton_.setBounds(controlsArea_.withLeft(controlsArea_.getCentreX()).withRight(controlsArea_.getCentreX() + controlsAreaHeight_).toNearestInt());
    playButton_.setBounds(controlsArea_.withLeft(controlsArea_.getCentreX() + controlsAreaHeight_).withRight(controlsArea_.getCentreX() + controlsAreaHeight_ * 2).toNearestInt());
    plusButton_.setBounds(controlsArea_.withLeft(controlsArea_.getRight() - controlsAreaHeight_).toNearestInt());
}

void YellowPlayer::changeAndHandleState(TransportState newState)
{
    if (newState == state_)
        return;
    
    state_ = newState;
    
    switch (state_) {
        case TransportState::Starting:
            transportSource_.start();

        case TransportState::Playing:
            startTimer(500);
            playButton_.setEnabled(false);
            pauseButton_.setEnabled(true);
            break;

        case TransportState::Pausing:
            transportSource_.stop();

        case TransportState::Paused:
            playButton_.setEnabled(true);
            pauseButton_.setEnabled(false);
            stopTimer();
            break;

        default:
            break;
    }
}

void YellowPlayer::plusButtonClicked() { 

    juce::FileChooser chooser ("Choose a song you want to play...",
                               juce::File::getSpecialLocation (juce::File::SpecialLocationType::userDocumentsDirectory),
                               "*.mp3; *.wav");
    
    if (chooser.browseForFileToOpen()) {
        juce::File songFile = chooser.getResult();

        auto* reader = formatManager_.createReaderFor(songFile);
        if (reader != nullptr) {
            auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
            transportSource_.setSource(newSource.get());
            readerSource_.reset(newSource.release());
        }
        
        playButton_.setEnabled(true);
    }
}

void YellowPlayer::pauseButtonClicked() { 
    changeAndHandleState(TransportState::Pausing);
}

void YellowPlayer::playButtonClicked() { 
    changeAndHandleState(TransportState::Starting);
}

void YellowPlayer::timerCallback() {
    repaint();
}
