/*
  ==============================================================================

    YellowPlayer.h
    Created: 18 Mar 2026 8:21:57am
    Author:  Yuliia Ruda

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "YellowPlayerLookAndFeel.h"
#include "SoundWavePath.h"

// //==============================================================================

class SoundLevelSlider : public juce::Slider {
public:
    SoundLevelSlider() : juce::Slider(juce::Slider::SliderStyle::LinearBar
                     , juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    {
        setRange(0.0, 100.0);
        setValue(50);
        setTextBoxIsEditable(false);
    };
    
    juce::String getTextFromValue (double value)
    {
        return juce::String(static_cast<int>(value)) + "%";
    }
};

//==============================================================================
/*
*/
class YellowPlayer  : public juce::AudioAppComponent,
                      public juce::ChangeListener,
                      private juce::Timer
{
public:
    YellowPlayer();
    ~YellowPlayer() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    enum TransportState {
        Paused,
        Starting,
        Playing,
        Pausing
    };
    
    void timerCallback () override;

private:
    void changeAndHandleState(TransportState newState);

    void plusButtonClicked();
    void playButtonClicked();
    void pauseButtonClicked();
    
    YellowPlayerLookAndFeel lookAndFeel;
    
    juce::Rectangle<float> centralArea_;
    juce::Rectangle<float> controlsArea_;       // bottom part with buttons and time
    juce::Rectangle<float> playerArea_;         // part where the song timeline is
    juce::Rectangle<float> underPlayersArea_;   // part between player and controls

    juce::Rectangle<float> topPlayerArea_;
    juce::Rectangle<float> bottomPlayerArea_;
    
    juce::Rectangle<float> timeArea_;

    SoundLevelSlider topSlider_;
    SoundLevelSlider bottomSlider_;

    juce::ShapeButton pauseButton_;
    juce::ShapeButton playButton_;
    juce::ShapeButton plusButton_;

    SoundWavePath topSoundWave_ = {YellowPlayerColours::white};

    const float controlsAreaHeight_ = 50.0f;
    const float helveticaMultiplier = 1.35f; // (height / ascent) + a little because the numbers are lower than ascent

    juce::AudioFormatManager formatManager_;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource_;
    juce::AudioTransportSource transportSource_;
    TransportState state_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (YellowPlayer)
};
