<<<<<<< HEAD
#pragma once

#include <JuceHeader.h>

// Forward declaration
class StreamlineAIProcessor;

class StreamlineAIEditor : public juce::AudioProcessorEditor,
                           public juce::Button::Listener,
                           public juce::TextEditor::Listener,
                           public juce::ComboBox::Listener,
                           public juce::Slider::Listener
{
public:
    StreamlineAIEditor(StreamlineAIProcessor&);
    ~StreamlineAIEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    // Button callbacks
    void buttonClicked(juce::Button* button) override;
    
    // Text editor callbacks
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override;
    
    // ComboBox callbacks
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    
    // Slider callbacks
    void sliderValueChanged(juce::Slider* slider) override;

private:
    // Reference to the processor
    StreamlineAIProcessor& audioProcessor;
    
    // UI Components
    juce::TextButton generateButton;
    juce::TextButton newGenerationButton;
    juce::Label resultLabel;
    
    // Follow-up questions UI
    juce::Label questionLabel;
    juce::ComboBox multipleChoiceOptions;
    juce::TextEditor textAnswer;
    juce::Slider numericSlider;
    juce::ComboBox durationTypeComboBox; // For seconds vs measures
    juce::TextButton nextButton;
    
    // Current state
    enum State { Initial, AskingQuestions, Generating, Complete };
    State currentState = Initial;
    
    // Questions management
    int currentQuestionIndex = 0;
    juce::StringArray questions;
    juce::Array<juce::StringArray> possibleAnswers;
    juce::Array<int> questionTypes; // 0 = multiple choice, 1 = text, 2 = numeric, 3 = duration
    juce::StringArray userAnswers;
    
    // Setup methods
    void setupInitialUI();
    void setupQuestionsUI();
    void setupCompletionUI(bool isMidi);
    void showNextQuestion();
    void submitAnswers();
    void startQuestionnaire();

    // Response display
    juce::TextEditor responseDisplay;
    
    // New method to create formatted response text directly from the collected answers
    juce::String createFormattedResponseText();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StreamlineAIEditor)
};
=======
#pragma once

#include <JuceHeader.h>

// Forward declaration
class StreamlineAIProcessor;

class StreamlineAIEditor : public juce::AudioProcessorEditor,
                           public juce::Button::Listener,
                           public juce::TextEditor::Listener,
                           public juce::ComboBox::Listener,
                           public juce::Slider::Listener
{
public:
    StreamlineAIEditor(StreamlineAIProcessor&);
    ~StreamlineAIEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    // Button callbacks
    void buttonClicked(juce::Button* button) override;
    
    // Text editor callbacks
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override;
    
    // ComboBox callbacks
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    
    // Slider callbacks
    void sliderValueChanged(juce::Slider* slider) override;

private:
    // Reference to the processor
    StreamlineAIProcessor& audioProcessor;
    
    // UI Components
    juce::TextButton generateButton;
    juce::TextButton newGenerationButton;
    juce::Label resultLabel;
    
    // Follow-up questions UI
    juce::Label questionLabel;
    juce::ComboBox multipleChoiceOptions;
    juce::TextEditor textAnswer;
    juce::Slider numericSlider;
    juce::ComboBox durationTypeComboBox; // For seconds vs measures
    juce::TextButton nextButton;
    
    // Current state
    enum State { Initial, AskingQuestions, Generating, Complete };
    State currentState = Initial;
    
    // Questions management
    int currentQuestionIndex = 0;
    juce::StringArray questions;
    juce::Array<juce::StringArray> possibleAnswers;
    juce::Array<int> questionTypes; // 0 = multiple choice, 1 = text, 2 = numeric, 3 = duration
    juce::StringArray userAnswers;
    
    // Setup methods
    void setupInitialUI();
    void setupQuestionsUI();
    void setupCompletionUI(bool isMidi);
    void showNextQuestion();
    void submitAnswers();
    void startQuestionnaire();

    // Response display
    juce::TextEditor responseDisplay;
    
    // New method to create formatted response text directly from the collected answers
    juce::String createFormattedResponseText();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StreamlineAIEditor)
};
>>>>>>> 732919b (Updated MVP, fixed readme, tweaked AI settings)
