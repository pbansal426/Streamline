// PluginEditor.cpp
/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
StreamlineAIAudioProcessorEditor::StreamlineAIAudioProcessorEditor (StreamlineAIAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      generateButton ("Generate")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to something sensible!
    setSize (400, 300);

    promptTextEditor.setMultiLine (false);
    promptTextEditor.setReturnKeyStartsNewLine (false);
    promptTextEditor.setReadOnly (false);
    promptTextEditor.setScrollbarsShown (true);
    promptTextEditor.setCaretVisible (true);
    promptTextEditor.setPopupMenuEnabled (true);
    promptTextEditor.setText ("Enter track description here...");
    addAndMakeVisible (promptTextEditor);

    generateButton.addListener (this);
    addAndMakeVisible (generateButton);
}

StreamlineAIAudioProcessorEditor::~StreamlineAIAudioProcessorEditor()
{
}

//==============================================================================
void StreamlineAIAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill it with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Streamline AI", getLocalBounds(), juce::Justification::centredTop, 1);
}

void StreamlineAIAudioProcessorEditor::resized()
{
    // This is generally where you'd lay out the positions of any
    // subcomponents in your editor..

    promptTextEditor.setBounds (20, 50, getWidth() - 40, 30);
    generateButton.setBounds (20, 90, 100, 30); // Position the button below the text box
}

void StreamlineAIAudioProcessorEditor::buttonClicked (juce::Button* button)
{
    if (button == &generateButton)
    {
        juce::String userPrompt = promptTextEditor.getText();
        audioProcessor.processPrompt(userPrompt); // Call a method in the processor
    }
}
