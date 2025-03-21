// PluginEditor.h
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
 */
class StreamlineAIAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                         public juce::Button::Listener
{
public:
    StreamlineAIAudioProcessorEditor (StreamlineAIAudioProcessor&);
    ~StreamlineAIAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void buttonClicked (juce::Button* button) override;

private:
    juce::Label responseLabel;
    // This reference is provided as a quick way to get access to the
    // plugin's parameter state.
    StreamlineAIAudioProcessor& audioProcessor;
    juce::TextEditor promptTextEditor;
    juce::TextButton generateButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StreamlineAIAudioProcessorEditor)
};
