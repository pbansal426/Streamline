<<<<<<< HEAD
#pragma once

#include <JuceHeader.h>

// Forward declaration
class StreamlineProcessor;

class StreamlineEditor : public juce::AudioProcessorEditor,
                         public juce::Button::Listener,
                         public juce::TextEditor::Listener,
                         public juce::ComboBox::Listener,
                         public juce::Slider::Listener
{
public:
    StreamlineEditor(StreamlineProcessor&);
    ~StreamlineEditor() override;

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
    StreamlineProcessor& audioProcessor;
    
    // UI Components
    juce::TextEditor promptInput;
    juce::TextButton generateButton;
    juce::Label statusLabel;
    
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
    
    // Project info display
    juce::Label projectInfoLabel;
    bool showProjectInfo = true;
    
    // Setup methods
    void setupInitialUI();
    void setupQuestionsUI();
    void showNextQuestion();
    void submitAnswers();
    void updateProjectInfoDisplay();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StreamlineEditor)
};

=======
#pragma once

#include <JuceHeader.h>

// Forward declaration
class StreamlineProcessor;

class StreamlineEditor : public juce::AudioProcessorEditor,
                         public juce::Button::Listener,
                         public juce::TextEditor::Listener,
                         public juce::ComboBox::Listener,
                         public juce::Slider::Listener
{
public:
    StreamlineEditor(StreamlineProcessor&);
    ~StreamlineEditor() override;

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
    StreamlineProcessor& audioProcessor;
    
    // UI Components
    juce::TextEditor promptInput;
    juce::TextButton generateButton;
    juce::Label statusLabel;
    
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
    
    // Project info display
    juce::Label projectInfoLabel;
    bool showProjectInfo = true;
    
    // Setup methods
    void setupInitialUI();
    void setupQuestionsUI();
    void showNextQuestion();
    void submitAnswers();
    void updateProjectInfoDisplay();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StreamlineEditor)
};

>>>>>>> 732919b (Updated MVP, fixed readme, tweaked AI settings)
