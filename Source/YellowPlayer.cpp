/*
  ==============================================================================

    YellowPlayer.cpp
    Created: 18 Mar 2026 8:21:57am
    Author:  Yuliia Ruda

  ==============================================================================
*/

#include <JuceHeader.h>
#include "YellowPlayer.h"

namespace {
// Layout and styling values, in pixels
namespace Layout {
    constexpr int compactHeightLimit = 650;             // the window is compact when it's this high or less

    constexpr float windowMargin = 20.0f;               // between window edge and the central area
    constexpr float controlsAreaHeight = 50.0f;         // bottom strip with the clock and the buttons

    constexpr float playerAreaCornerRadius = 20.0f;     // the big central rounded rectangle
    constexpr float innerPanelCornerInset = 5.0f;       // inner panels are less round than the area around them
    constexpr float innerPanelCornerRadius = playerAreaCornerRadius - innerPanelCornerInset;

    constexpr float playerHorizontalInset = 10.0f;      // between the central area edge and each wave panel
    constexpr float playerVerticalInset = 7.5f;         // between the top/bottom edges of a half and its wave panel
    constexpr float playerVerticalNudge = 2.5f;         // pushes the wave panels apart (top moves down, bottom up)

    constexpr float sliderHorizontalInset = 10.0f;      // between the wave panel edge and the volume slider
    constexpr float sliderHeight = 20.0f;               // normal slider (with the percentage text)
    constexpr float sliderHeightCompact = 5.0f;         // thin line slider
    constexpr float sliderBottomMargin = 15.0f;         // between the slider and the bottom of its wave panel
    constexpr float sliderBottomMarginCompact = 10.0f;

    constexpr float channelLabelAreaHeight = 30.0f;     // strip on top of each wave panel with the channel label
    constexpr float channelLabelExtraTopMargin = 5.0f; // extra room above the waveform for the channel label
    constexpr float waveExtraTopMarginCompact = 10.0f;  // room above the time stamps when there are no channel labels
}
}

//==============================================================================
YellowPlayer::YellowPlayer()
    : state_(TransportState::Paused)
    , pauseButton_("Pause", YellowPlayerColours::lime, YellowPlayerColours::lime.brighter(), YellowPlayerColours::lime)
    , playButton_("Play", YellowPlayerColours::lime, YellowPlayerColours::lime.brighter(), YellowPlayerColours::lime)
    , plusButton_("Add song", YellowPlayerColours::lime, YellowPlayerColours::lime.brighter(), YellowPlayerColours::lime)
{
    setAudioChannels (/* input channels */0, /*output channels */2);
    formatManager_.registerBasicFormats();
    transportSource_.addChangeListener(this);
    
    setLookAndFeel(&lookAndFeel);

    // Just to force to recreate text box using the custom look and feel of the parent
    topSlider_.setLookAndFeel(&lookAndFeel);
    bottomSlider_.setLookAndFeel(&lookAndFeel);
    topSlider_.onValueChange = [this]() {
        topChannelVolume_ = topSlider_.getValue();
    };
    bottomSlider_.onValueChange = [this]() {
        bottomChannelVolume_ = bottomSlider_.getValue();
    };

    float buttonSize = 50.0f;
    float linesWidth = 10.0f;

    pauseButton_.setShape(YellowPlayerLookAndFeel::pauseShape(buttonSize, 30.f, linesWidth), true, true, true);
    playButton_.setShape(YellowPlayerLookAndFeel::playShape(buttonSize, buttonSize * 0.75f), true, true, true);
    plusButton_.setShape(YellowPlayerLookAndFeel::plusShape(buttonSize, linesWidth), true, true, true);

    addAndMakeVisible(topSlider_);
    addAndMakeVisible(bottomSlider_);
    addAndMakeVisible(topSoundWave_);
    addAndMakeVisible(bottomSoundWave_);

    addAndMakeVisible(pauseButton_);
    addAndMakeVisible(playButton_);
    addAndMakeVisible(plusButton_);

    topSoundWave_.onZoom = [this](juce::Range<double> range){
        topSoundWave_.resetTimeBounds(range.getStart(), range.getEnd());
        bottomSoundWave_.resetTimeBounds(range.getStart(), range.getEnd());
    };
    bottomSoundWave_.onZoom = [this](juce::Range<double> range){
        topSoundWave_.resetTimeBounds(range.getStart(), range.getEnd());
        bottomSoundWave_.resetTimeBounds(range.getStart(), range.getEnd());
    };

    auto onSeek = [this](double time) {
        transportSource_.setPosition(time);
        setCurrentTimeInBothWaveforms(time);
    };
    topSoundWave_.onSeekRequested = onSeek;
    bottomSoundWave_.onSeekRequested = onSeek;

    plusButton_.onClick = [this]{ plusButtonClicked(); };
    playButton_.onClick = [this]{ playButtonClicked(); };
    pauseButton_.onClick = [this]{ pauseButtonClicked(); };

    playButton_.setEnabled(false);
    pauseButton_.setEnabled(false);

    setSize(800, 800);
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
    transportSource_.getNextAudioBlock(bufferToFill);

    auto leftBuff = bufferToFill.buffer->getWritePointer(0);
    auto rightBuff = bufferToFill.buffer->getWritePointer(1);
    for (int smpl = 0; smpl < bufferToFill.numSamples; ++smpl) {
        leftBuff[bufferToFill.startSample + smpl] = leftBuff[bufferToFill.startSample + smpl] * topChannelVolume_;
        rightBuff[bufferToFill.startSample + smpl] = rightBuff[bufferToFill.startSample + smpl] * bottomChannelVolume_;
    }
}

void YellowPlayer::releaseResources()
{
    transportSource_.releaseResources();
}

//========================== PLAY / PAUSE LOGIC =================================

void YellowPlayer::changeAndHandleState(TransportState newState)
{
    if (newState == state_)
        return;
    
    state_ = newState;
    
    switch (state_) {
        case TransportState::Starting:
            transportSource_.start();
            break;

        case TransportState::Playing:
            if (!isTimerRunning())
                startTimerHz(30);
            playButton_.setEnabled(false);
            pauseButton_.setEnabled(true);
            break;

        case TransportState::Pausing:
            transportSource_.stop();
            break;

        case TransportState::Paused:
            playButton_.setEnabled(true);
            pauseButton_.setEnabled(false);
            setCurrentTimeInBothWaveforms(transportSource_.getCurrentPosition());
            stopTimer();
            break;
            
        case TransportState::Finished:
            playButton_.setEnabled(true);
            pauseButton_.setEnabled(false);
            stopTimer();
            transportSource_.setPosition(0.0f);
            setCurrentTimeInBothWaveforms(0.0);
            if (auto* peer = getPeer())
                peer->performAnyPendingRepaintsNow();
            break;

        default:
            break;
    }
}

void YellowPlayer::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &transportSource_)
    {
        if (transportSource_.isPlaying()) {
            changeAndHandleState(TransportState::Playing);
        } else {
            if (transportSource_.hasStreamFinished()) {
                changeAndHandleState(TransportState::Finished);
            } else {
                changeAndHandleState(TransportState::Paused);
            }
        }
    }
}

// ============================ GUI ============================

void YellowPlayer::paint (juce::Graphics& g)
{
    // Background
    g.fillAll (YellowPlayerColours::black);

    // Central component
    g.setColour (YellowPlayerColours::darkGrey);
    g.fillRoundedRectangle(playerArea_, Layout::playerAreaCornerRadius);

    // Player components
    g.setColour(YellowPlayerColours::black.brighter());
    g.fillRoundedRectangle(topPlayerArea_, Layout::innerPanelCornerRadius);
    g.fillRoundedRectangle(bottomPlayerArea_, Layout::innerPanelCornerRadius);

    // Time
    auto clockAreaDrawingBounds = timeArea_.reduced(2.0f);
    g.setColour(lookAndFeel.findColour(juce::Label::textColourId));

    juce::RelativeTime currentTime(transportSource_.getCurrentPosition());
    juce::String timeString = juce::String (juce::String((int)currentTime.inMinutes()).paddedLeft('0', 2) + ":" + (juce::String((int)currentTime.inSeconds() % 60)).paddedLeft('0', 2));
    auto clockFont = YellowPlayerLookAndFeel::getHelveticaForHeight(Layout::controlsAreaHeight);
    g.setFont(clockFont);
    g.drawSingleLineText (timeString, clockAreaDrawingBounds.getX(), clockAreaDrawingBounds.getBottom());

    // Channel labels (top wave is the left channel, bottom is the right one)
    if (!compact_) {
        paintChannelLabel(g, topLabelArea_, "Left channel");
        paintChannelLabel(g, bottomLabelArea_, "Right channel");
    }
}

#if YELLOWPLAYER_SHOW_BOUNDS
// Thin red outlines of every area and child component
void YellowPlayer::paintOverChildren(juce::Graphics& g)
{
    g.setColour(juce::Colours::red);

    for (const auto& area : { centralArea_, controlsArea_, playerArea_, underPlayersArea_,
                              topPlayerArea_, bottomPlayerArea_, topLabelArea_, bottomLabelArea_, timeArea_ })
        g.drawRect(area, 1.0f);

    for (auto* child : getChildren())
        g.drawRect(child->getBounds().toFloat(), 1.0f);
}
#endif

void YellowPlayer::paintChannelLabel(juce::Graphics& g, const juce::Rectangle<float>& labelArea, const juce::String& text)
{
    constexpr float iconSize = 17.0f;
    constexpr float iconGap = 7.0f;

    // Icon and text inside a rounded rectangle start one corner radius away from its left edge
    auto area = labelArea.withTrimmedLeft(Layout::innerPanelCornerRadius + YellowPlayerStyle::triangleSize);
    g.setColour(YellowPlayerColours::lime);

    // Speaker icon sits at the bottom of the label area, like the text.
    // The black PNG is used as a mask, so it gets painted with the current (lime) colour
    auto iconBounds = area.removeFromLeft(iconSize).withTop(area.getBottom() - iconSize);
    {
        juce::Graphics::ScopedSaveState state(g);
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        g.drawImage(speakerIcon_, iconBounds, juce::RectanglePlacement::stretchToFit, true);
    }

    area.removeFromLeft(iconGap);
    g.setFont(YellowPlayerLookAndFeel::getHelveticaForHeight(10.0f).withStyle("Bold"));
    g.drawText(text, area, juce::Justification::bottomLeft);
}

void YellowPlayer::resized()
{
    // Big areas
    centralArea_ = getLocalBounds().toFloat().reduced(Layout::windowMargin);

    // Small window: volume sliders become thin lines without numbers
    compact_ = getHeight() <= Layout::compactHeightLimit;
    sliderHeight_ = compact_ ? Layout::sliderHeightCompact : Layout::sliderHeight;
    topSlider_.setCompact(compact_);
    bottomSlider_.setCompact(compact_);

    const float sliderBottomMargin = compact_ ? Layout::sliderBottomMarginCompact : Layout::sliderBottomMargin;

    // [ the order of these is important
    auto centralAreaCopy = centralArea_;
    controlsArea_ = centralAreaCopy.removeFromBottom(Layout::controlsAreaHeight);  // bottom 10% in original size
    underPlayersArea_ = centralAreaCopy.removeFromBottom(Layout::controlsAreaHeight);
    playerArea_ = centralAreaCopy;  // all area - controlsArea - underPlayersArea

    // Player area components
    topPlayerArea_ = centralAreaCopy.removeFromTop(playerArea_.getHeight() / 2).reduced(Layout::playerHorizontalInset, Layout::playerVerticalInset).translated(0, Layout::playerVerticalNudge);
    bottomPlayerArea_ = centralAreaCopy.removeFromTop(playerArea_.getHeight() / 2).reduced(Layout::playerHorizontalInset, Layout::playerVerticalInset).translated(0, -Layout::playerVerticalNudge);
    // closing the area cutting sequence ]

    topSlider_.setBounds(topPlayerArea_.withHeight(sliderHeight_).withBottomY(topPlayerArea_.getBottom() - sliderBottomMargin).reduced(Layout::sliderHorizontalInset, 0).toNearestInt());
    bottomSlider_.setBounds(bottomPlayerArea_.withHeight(sliderHeight_).withBottomY(bottomPlayerArea_.getBottom() - sliderBottomMargin).reduced(Layout::sliderHorizontalInset, 0).toNearestInt());

    // No channel labels in compact mode, so the label areas take no space
    const float labelAreaHeight = compact_ ? 0.0f : Layout::channelLabelAreaHeight;
    topLabelArea_.setBounds(topPlayerArea_.getX(), topPlayerArea_.getY(), topPlayerArea_.getWidth(), labelAreaHeight);
    bottomLabelArea_.setBounds(bottomPlayerArea_.getX(), bottomPlayerArea_.getY(), bottomPlayerArea_.getWidth(), labelAreaHeight);

    // Must be set before the waves' setBounds()
    const float waveExtraTopMargin = compact_ ? Layout::waveExtraTopMarginCompact : Layout::channelLabelExtraTopMargin;
    topSoundWave_.setExtraTopMargin(waveExtraTopMargin);
    bottomSoundWave_.setExtraTopMargin(waveExtraTopMargin);

    topSoundWave_.setBounds(topLabelArea_.getX(), topLabelArea_.getBottom(), topLabelArea_.getWidth(), topSlider_.getY() - topLabelArea_.getBottom());
    bottomSoundWave_.setBounds(bottomLabelArea_.getX(), bottomLabelArea_.getBottom(), bottomLabelArea_.getWidth(), bottomSlider_.getY() - bottomLabelArea_.getBottom());

    // Controls area components
    auto controlsAreaCopy = controlsArea_;
    timeArea_ = controlsAreaCopy.removeFromLeft(controlsArea_.getWidth() / 2.0f);

    pauseButton_.setBounds(controlsArea_.withLeft(controlsArea_.getCentreX()).withRight(controlsArea_.getCentreX() + Layout::controlsAreaHeight).toNearestInt());
    playButton_.setBounds(controlsArea_.withLeft(controlsArea_.getCentreX() + Layout::controlsAreaHeight).withRight(controlsArea_.getCentreX() + Layout::controlsAreaHeight * 2).toNearestInt());
    plusButton_.setBounds(controlsArea_.withLeft(controlsArea_.getRight() - Layout::controlsAreaHeight).toNearestInt());
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
            transportSource_.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
            readerSource_.reset(newSource.release());

            topSoundWave_.setSourceForWaveform(new juce::FileInputSource(songFile));
            bottomSoundWave_.setSourceForWaveform(new juce::FileInputSource(songFile));
            setCurrentTimeInBothWaveforms(0.0);
            topSoundWave_.repaint();
            bottomSoundWave_.repaint();
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
    if (!isShowing())
        return;

    bool shouldRepaint = true;

    if (!juce::Process::isForegroundProcess()) {
        if (repaintWhenBackgroundCounter_ % 5 != 0)
            shouldRepaint = false;

        repaintWhenBackgroundCounter_++;
    } else {
        repaintWhenBackgroundCounter_ = 0;
    }
    
    if (!shouldRepaint)
        return;

    const double pos = transportSource_.getCurrentPosition();
    if (!topSoundWave_.isDragging())
        topSoundWave_.setCurrentTime(pos);
    if (!bottomSoundWave_.isDragging())
        bottomSoundWave_.setCurrentTime(pos);

    repaint();
}

void YellowPlayer::setCurrentTimeInBothWaveforms(double timeInSec) {
    topSoundWave_.setCurrentTime(timeInSec);
    bottomSoundWave_.setCurrentTime(timeInSec);
}
