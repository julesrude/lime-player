/*
  ==============================================================================

    MainComponent.h
    Created: 16 Mar 2026 8:24:34pm
    Author:  Yuliia Ruda

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class MainComponent  : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    juce::FontOptions getHeadingFont();
    juce::FontOptions getTextFont();
    float fontSize = 40.f;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
