<<<<<<< HEAD
#include "StreamlineProcessor.h"
#include "StreamlineEditor.h"

StreamlineEditor::StreamlineEditor(StreamlineProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set up the initial UI
    setupInitialUI();
    
    // Set up callbacks for when generation is complete
    audioProcessor.onMidiGenerated = [this](const juce::MidiFile& midi) {
        statusLabel.setText("Generation complete! MIDI file created.", juce::dontSendNotification);
        currentState = Complete;
        repaint();
    };
    
    audioProcessor.onAudioGenerated = [this](const juce::AudioBuffer<float>& audio) {
        statusLabel.setText("Generation complete! Audio created.", juce::dontSendNotification);
        currentState = Complete;
        repaint();
    };
    
    // Set up callback for when AI generates questions
    audioProcessor.onQuestionsGenerated = [this](const juce::StringArray& newQuestions,
                                                const juce::Array<juce::StringArray>& newPossibleAnswers,
                                                const juce::Array<int>& newQuestionTypes) {
        // Store the questions and possible answers
        questions = newQuestions;
        possibleAnswers = newPossibleAnswers;
        questionTypes = newQuestionTypes;
        
        // Start the question sequence
        currentState = AskingQuestions;
        currentQuestionIndex = 0;
        userAnswers.clear();
        
        // Set up the questions UI
        setupQuestionsUI();
        showNextQuestion();
    };
    
    // Set the size of the plugin UI
    setSize(600, 400);
}

StreamlineEditor::~StreamlineEditor()
{
}

void StreamlineEditor::paint(juce::Graphics& g)
{
    // Fill the background
    g.fillAll(juce::Colours::darkgrey);
    
    // Add a title
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("Streamline AI", getLocalBounds().reduced(10), juce::Justification::top, true);
}

void StreamlineEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    
    // Position the components based on the current state
    if (currentState == Initial)
    {
        // Title area
        area.removeFromTop(30);
        
        // Project info display
        if (showProjectInfo)
        {
            projectInfoLabel.setBounds(area.removeFromTop(60));
            area.removeFromTop(10);
        }
        
        // Prompt input
        promptInput.setBounds(area.removeFromTop(40));
        area.removeFromTop(10);
        
        // Generate button
        generateButton.setBounds(area.removeFromTop(30).withSizeKeepingCentre(120, 30));
        area.removeFromTop(10);
        
        // Status label
        statusLabel.setBounds(area.removeFromTop(30));
    }
    else if (currentState == AskingQuestions)
    {
        // Title area
        area.removeFromTop(30);
        
        // Question label
        questionLabel.setBounds(area.removeFromTop(30));
        area.removeFromTop(10);
        
        // Answer components based on question type
        if (currentQuestionIndex < questionTypes.size())
        {
            int questionType = questionTypes[currentQuestionIndex];
            
            if (questionType == 0) // Multiple choice
            {
                // Multiple choice
                multipleChoiceOptions.setBounds(area.removeFromTop(30));
                textAnswer.setVisible(false);
                numericSlider.setVisible(false);
                durationTypeComboBox.setVisible(false);
            }
            else if (questionType == 1) // Text input
            {
                // Text input
                textAnswer.setBounds(area.removeFromTop(60));
                multipleChoiceOptions.setVisible(false);
                numericSlider.setVisible(false);
                durationTypeComboBox.setVisible(false);
            }
            else if (questionType == 2) // Numeric slider
            {
                // Numeric slider
                numericSlider.setBounds(area.removeFromTop(40));
                multipleChoiceOptions.setVisible(false);
                textAnswer.setVisible(false);
                durationTypeComboBox.setVisible(false);
            }
            else if (questionType == 3) // Duration with type selection
            {
                // Duration input with type selection
                auto durationRow = area.removeFromTop(30);
                numericSlider.setBounds(durationRow.removeFromLeft(durationRow.getWidth() * 0.7f));
                durationTypeComboBox.setBounds(durationRow);
                
                multipleChoiceOptions.setVisible(false);
                textAnswer.setVisible(false);
            }
        }
        
        area.removeFromTop(10);
        
        // Next button
        nextButton.setBounds(area.removeFromTop(30).withSizeKeepingCentre(120, 30));
        area.removeFromTop(10);
        
        // Status label
        statusLabel.setBounds(area.removeFromTop(30));
    }
    else if (currentState == Generating || currentState == Complete)
    {
        // Title area
        area.removeFromTop(30);
        
        // Status label
        statusLabel.setBounds(area.removeFromTop(30));
        
        // If complete, add a "New Generation" button
        if (currentState == Complete)
        {
            area.removeFromTop(10);
            generateButton.setBounds(area.removeFromTop(30).withSizeKeepingCentre(150, 30));
        }
    }
}

void StreamlineEditor::buttonClicked(juce::Button* button)
{
    if (button == &generateButton)
    {
        if (currentState == Initial)
        {
            // Get the prompt and start the generation process
            juce::String prompt = promptInput.getText();
            
            // Update status
            statusLabel.setText("Processing prompt...", juce::dontSendNotification);
            
            // Call the processor to generate music from the prompt
            // This will trigger the onQuestionsGenerated callback
            audioProcessor.generateMusicFromPrompt(prompt);
        }
        else if (currentState == Complete)
        {
            // Reset to initial state for a new generation
            currentState = Initial;
            setupInitialUI();
        }
    }
    else if (button == &nextButton)
    {
        // Save the current answer based on the question type
        if (currentQuestionIndex < questionTypes.size())
        {
            int questionType = questionTypes[currentQuestionIndex];
            
            if (questionType == 0) // Multiple choice
            {
                userAnswers.add(multipleChoiceOptions.getText());
            }
            else if (questionType == 1) // Text input
            {
                userAnswers.add(textAnswer.getText());
            }
            else if (questionType == 2) // Numeric slider
            {
                userAnswers.add(juce::String(numericSlider.getValue()));
            }
            else if (questionType == 3) // Duration with type selection
            {
                // Combine the numeric value with the duration type
                juce::String durationValue = juce::String(numericSlider.getValue());
                juce::String durationType = durationTypeComboBox.getText();
                userAnswers.add(durationValue + " " + durationType);
            }
        }
        
        // Move to the next question or submit if done
        currentQuestionIndex++;
        
        if (currentQuestionIndex < questions.size())
        {
            showNextQuestion();
        }
        else
        {
            submitAnswers();
        }
    }
}

void StreamlineEditor::textEditorReturnKeyPressed(juce::TextEditor& editor)
{
    if (&editor == &promptInput && currentState == Initial)
    {
        // Same as clicking the generate button
        buttonClicked(&generateButton);
    }
    else if (&editor == &textAnswer && currentState == AskingQuestions)
    {
        // Same as clicking the next button
        buttonClicked(&nextButton);
    }
}

void StreamlineEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    // Handle combo box changes if needed
}

void StreamlineEditor::sliderValueChanged(juce::Slider* slider)
{
    // Handle slider changes if needed
}

void StreamlineEditor::setupInitialUI()
{
    // Remove all existing components
    removeAllChildren();
    
    // Add and configure the project info label
    addAndMakeVisible(projectInfoLabel);
    updateProjectInfoDisplay();
    
    // Add and configure the prompt input
    addAndMakeVisible(promptInput);
    promptInput.setMultiLine(false);
    promptInput.setReturnKeyStartsNewLine(false);
    promptInput.setReadOnly(false);
    promptInput.setScrollbarsShown(true);
    promptInput.setCaretVisible(true);
    promptInput.setPopupMenuEnabled(true);
    promptInput.setText("Enter a prompt (e.g., 'electric guitar solo')");
    promptInput.addListener(this);
    
    // Add and configure the generate button
    addAndMakeVisible(generateButton);
    generateButton.setButtonText("Generate");
    generateButton.addListener(this);
    
    // Add and configure the status label
    addAndMakeVisible(statusLabel);
    statusLabel.setText("Enter a prompt to begin", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    
    // Update the layout
    resized();
}

void StreamlineEditor::setupQuestionsUI()
{
    // Remove all existing components
    removeAllChildren();
    
    // Add and configure the question label
    addAndMakeVisible(questionLabel);
    questionLabel.setJustificationType(juce::Justification::centred);
    
    // Add and configure the multiple choice options
    addAndMakeVisible(multipleChoiceOptions);
    multipleChoiceOptions.addListener(this);
    
    // Add and configure the text answer
    addAndMakeVisible(textAnswer);
    textAnswer.setMultiLine(true);
    textAnswer.setReturnKeyStartsNewLine(true);
    textAnswer.setReadOnly(false);
    textAnswer.setScrollbarsShown(true);
    textAnswer.setCaretVisible(true);
    textAnswer.setPopupMenuEnabled(true);
    textAnswer.addListener(this);
    
    // Add and configure the numeric slider
    addAndMakeVisible(numericSlider);
    numericSlider.setRange(1.0, 10.0, 1.0);
    numericSlider.setValue(5.0);
    numericSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    numericSlider.addListener(this);
    
    // Add and configure the duration type combo box
    addAndMakeVisible(durationTypeComboBox);
    durationTypeComboBox.addItem("Seconds", 1);
    durationTypeComboBox.addItem("Measures", 2);
    durationTypeComboBox.setSelectedItemIndex(0);
    durationTypeComboBox.addListener(this);
    
    // Add and configure the next button
    addAndMakeVisible(nextButton);
    nextButton.setButtonText("Next");
    nextButton.addListener(this);
    
    // Add and configure the status label
    addAndMakeVisible(statusLabel);
    statusLabel.setText("Answer the questions to refine your generation", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    
    // Update the layout
    resized();
}

void StreamlineEditor::showNextQuestion()
{
    // Update the question label
    questionLabel.setText(questions[currentQuestionIndex], juce::dontSendNotification);
    
    // Set up the answer component based on the question type
    if (currentQuestionIndex < questionTypes.size())
    {
        int questionType = questionTypes[currentQuestionIndex];
        
        if (questionType == 0) // Multiple choice
        {
            // Multiple choice question
            multipleChoiceOptions.clear();
            multipleChoiceOptions.setVisible(true);
            textAnswer.setVisible(false);
            numericSlider.setVisible(false);
            durationTypeComboBox.setVisible(false);
            
            // Add the options
            int itemId = 1;
            
            for (auto& option : possibleAnswers[currentQuestionIndex])
            {
                multipleChoiceOptions.addItem(option, itemId++);
            }
            
            // Select the first option by default
            multipleChoiceOptions.setSelectedItemIndex(0);
        }
        else if (questionType == 1) // Text input
        {
            // Text input question
            multipleChoiceOptions.setVisible(false);
            textAnswer.setVisible(true);
            numericSlider.setVisible(false);
            durationTypeComboBox.setVisible(false);
            
            textAnswer.setText("", false);
            textAnswer.setTextToShowWhenEmpty("Enter your answer", juce::Colours::grey);
        }
        else if (questionType == 2) // Numeric slider
        {
            // Numeric slider question
            multipleChoiceOptions.setVisible(false);
            textAnswer.setVisible(false);
            numericSlider.setVisible(true);
            durationTypeComboBox.setVisible(false);
            
            // Configure the slider based on the question
            if (questions[currentQuestionIndex].contains("1-10"))
            {
                numericSlider.setRange(1.0, 10.0, 1.0);
                numericSlider.setValue(5.0);
            }
            else
            {
                numericSlider.setRange(1.0, 100.0, 1.0);
                numericSlider.setValue(50.0);
            }
        }
        else if (questionType == 3) // Duration with type selection
        {
            // Duration input with type selection
            multipleChoiceOptions.setVisible(false);
            textAnswer.setVisible(false);
            numericSlider.setVisible(true);
            durationTypeComboBox.setVisible(true);
            
            // Configure the slider for duration
            numericSlider.setRange(1.0, 60.0, 1.0);
            numericSlider.setValue(16.0); // Default to 16 measures or seconds
            
            // Make sure the duration type is set
            if (durationTypeComboBox.getSelectedId() == 0)
                durationTypeComboBox.setSelectedItemIndex(0);
        }
    }
    
    // Update the layout
    resized();
}

void StreamlineEditor::submitAnswers()
{
    // Change state to generating
    currentState = Generating;
    
    // Update UI
    removeAllChildren();
    addAndMakeVisible(statusLabel);
    statusLabel.setText("Generating music...", juce::dontSendNotification);
    resized();
    
    // Send the answers to the processor
    audioProcessor.processFollowUpAnswers(userAnswers);
}

void StreamlineEditor::updateProjectInfoDisplay()
{
    // Get project information from the processor
    double tempo = audioProcessor.getProjectTempo();
    int timeSigNum = audioProcessor.getProjectTimeSigNumerator();
    int timeSigDenom = audioProcessor.getProjectTimeSigDenominator();
    
    // Create a string with the project info
    juce::String infoText = "Project Info:\n";
    infoText += "Tempo: " + juce::String(tempo) + " BPM\n";
    infoText += "Time Signature: " + juce::String(timeSigNum) + "/" + juce::String(timeSigDenom);
    
    // Update the label
    projectInfoLabel.setText(infoText, juce::dontSendNotification);
}
=======
#include "StreamlineProcessor.h"
#include "StreamlineEditor.h"

StreamlineEditor::StreamlineEditor(StreamlineProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set up the initial UI
    setupInitialUI();
    
    // Set up callbacks for when generation is complete
    audioProcessor.onMidiGenerated = [this](const juce::MidiFile& midi) {
        statusLabel.setText("Generation complete! MIDI file created.", juce::dontSendNotification);
        currentState = Complete;
        repaint();
    };
    
    audioProcessor.onAudioGenerated = [this](const juce::AudioBuffer<float>& audio) {
        statusLabel.setText("Generation complete! Audio created.", juce::dontSendNotification);
        currentState = Complete;
        repaint();
    };
    
    // Set up callback for when AI generates questions
    audioProcessor.onQuestionsGenerated = [this](const juce::StringArray& newQuestions,
                                                const juce::Array<juce::StringArray>& newPossibleAnswers,
                                                const juce::Array<int>& newQuestionTypes) {
        // Store the questions and possible answers
        questions = newQuestions;
        possibleAnswers = newPossibleAnswers;
        questionTypes = newQuestionTypes;
        
        // Start the question sequence
        currentState = AskingQuestions;
        currentQuestionIndex = 0;
        userAnswers.clear();
        
        // Set up the questions UI
        setupQuestionsUI();
        showNextQuestion();
    };
    
    // Set the size of the plugin UI
    setSize(600, 400);
}

StreamlineEditor::~StreamlineEditor()
{
}

void StreamlineEditor::paint(juce::Graphics& g)
{
    // Fill the background
    g.fillAll(juce::Colours::darkgrey);
    
    // Add a title
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("Streamline AI", getLocalBounds().reduced(10), juce::Justification::top, true);
}

void StreamlineEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    
    // Position the components based on the current state
    if (currentState == Initial)
    {
        // Title area
        area.removeFromTop(30);
        
        // Project info display
        if (showProjectInfo)
        {
            projectInfoLabel.setBounds(area.removeFromTop(60));
            area.removeFromTop(10);
        }
        
        // Prompt input
        promptInput.setBounds(area.removeFromTop(40));
        area.removeFromTop(10);
        
        // Generate button
        generateButton.setBounds(area.removeFromTop(30).withSizeKeepingCentre(120, 30));
        area.removeFromTop(10);
        
        // Status label
        statusLabel.setBounds(area.removeFromTop(30));
    }
    else if (currentState == AskingQuestions)
    {
        // Title area
        area.removeFromTop(30);
        
        // Question label
        questionLabel.setBounds(area.removeFromTop(30));
        area.removeFromTop(10);
        
        // Answer components based on question type
        if (currentQuestionIndex < questionTypes.size())
        {
            int questionType = questionTypes[currentQuestionIndex];
            
            if (questionType == 0) // Multiple choice
            {
                // Multiple choice
                multipleChoiceOptions.setBounds(area.removeFromTop(30));
                textAnswer.setVisible(false);
                numericSlider.setVisible(false);
                durationTypeComboBox.setVisible(false);
            }
            else if (questionType == 1) // Text input
            {
                // Text input
                textAnswer.setBounds(area.removeFromTop(60));
                multipleChoiceOptions.setVisible(false);
                numericSlider.setVisible(false);
                durationTypeComboBox.setVisible(false);
            }
            else if (questionType == 2) // Numeric slider
            {
                // Numeric slider
                numericSlider.setBounds(area.removeFromTop(40));
                multipleChoiceOptions.setVisible(false);
                textAnswer.setVisible(false);
                durationTypeComboBox.setVisible(false);
            }
            else if (questionType == 3) // Duration with type selection
            {
                // Duration input with type selection
                auto durationRow = area.removeFromTop(30);
                numericSlider.setBounds(durationRow.removeFromLeft(durationRow.getWidth() * 0.7f));
                durationTypeComboBox.setBounds(durationRow);
                
                multipleChoiceOptions.setVisible(false);
                textAnswer.setVisible(false);
            }
        }
        
        area.removeFromTop(10);
        
        // Next button
        nextButton.setBounds(area.removeFromTop(30).withSizeKeepingCentre(120, 30));
        area.removeFromTop(10);
        
        // Status label
        statusLabel.setBounds(area.removeFromTop(30));
    }
    else if (currentState == Generating || currentState == Complete)
    {
        // Title area
        area.removeFromTop(30);
        
        // Status label
        statusLabel.setBounds(area.removeFromTop(30));
        
        // If complete, add a "New Generation" button
        if (currentState == Complete)
        {
            area.removeFromTop(10);
            generateButton.setBounds(area.removeFromTop(30).withSizeKeepingCentre(150, 30));
        }
    }
}

void StreamlineEditor::buttonClicked(juce::Button* button)
{
    if (button == &generateButton)
    {
        if (currentState == Initial)
        {
            // Get the prompt and start the generation process
            juce::String prompt = promptInput.getText();
            
            // Update status
            statusLabel.setText("Processing prompt...", juce::dontSendNotification);
            
            // Call the processor to generate music from the prompt
            // This will trigger the onQuestionsGenerated callback
            audioProcessor.generateMusicFromPrompt(prompt);
        }
        else if (currentState == Complete)
        {
            // Reset to initial state for a new generation
            currentState = Initial;
            setupInitialUI();
        }
    }
    else if (button == &nextButton)
    {
        // Save the current answer based on the question type
        if (currentQuestionIndex < questionTypes.size())
        {
            int questionType = questionTypes[currentQuestionIndex];
            
            if (questionType == 0) // Multiple choice
            {
                userAnswers.add(multipleChoiceOptions.getText());
            }
            else if (questionType == 1) // Text input
            {
                userAnswers.add(textAnswer.getText());
            }
            else if (questionType == 2) // Numeric slider
            {
                userAnswers.add(juce::String(numericSlider.getValue()));
            }
            else if (questionType == 3) // Duration with type selection
            {
                // Combine the numeric value with the duration type
                juce::String durationValue = juce::String(numericSlider.getValue());
                juce::String durationType = durationTypeComboBox.getText();
                userAnswers.add(durationValue + " " + durationType);
            }
        }
        
        // Move to the next question or submit if done
        currentQuestionIndex++;
        
        if (currentQuestionIndex < questions.size())
        {
            showNextQuestion();
        }
        else
        {
            submitAnswers();
        }
    }
}

void StreamlineEditor::textEditorReturnKeyPressed(juce::TextEditor& editor)
{
    if (&editor == &promptInput && currentState == Initial)
    {
        // Same as clicking the generate button
        buttonClicked(&generateButton);
    }
    else if (&editor == &textAnswer && currentState == AskingQuestions)
    {
        // Same as clicking the next button
        buttonClicked(&nextButton);
    }
}

void StreamlineEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    // Handle combo box changes if needed
}

void StreamlineEditor::sliderValueChanged(juce::Slider* slider)
{
    // Handle slider changes if needed
}

void StreamlineEditor::setupInitialUI()
{
    // Remove all existing components
    removeAllChildren();
    
    // Add and configure the project info label
    addAndMakeVisible(projectInfoLabel);
    updateProjectInfoDisplay();
    
    // Add and configure the prompt input
    addAndMakeVisible(promptInput);
    promptInput.setMultiLine(false);
    promptInput.setReturnKeyStartsNewLine(false);
    promptInput.setReadOnly(false);
    promptInput.setScrollbarsShown(true);
    promptInput.setCaretVisible(true);
    promptInput.setPopupMenuEnabled(true);
    promptInput.setText("Enter a prompt (e.g., 'electric guitar solo')");
    promptInput.addListener(this);
    
    // Add and configure the generate button
    addAndMakeVisible(generateButton);
    generateButton.setButtonText("Generate");
    generateButton.addListener(this);
    
    // Add and configure the status label
    addAndMakeVisible(statusLabel);
    statusLabel.setText("Enter a prompt to begin", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    
    // Update the layout
    resized();
}

void StreamlineEditor::setupQuestionsUI()
{
    // Remove all existing components
    removeAllChildren();
    
    // Add and configure the question label
    addAndMakeVisible(questionLabel);
    questionLabel.setJustificationType(juce::Justification::centred);
    
    // Add and configure the multiple choice options
    addAndMakeVisible(multipleChoiceOptions);
    multipleChoiceOptions.addListener(this);
    
    // Add and configure the text answer
    addAndMakeVisible(textAnswer);
    textAnswer.setMultiLine(true);
    textAnswer.setReturnKeyStartsNewLine(true);
    textAnswer.setReadOnly(false);
    textAnswer.setScrollbarsShown(true);
    textAnswer.setCaretVisible(true);
    textAnswer.setPopupMenuEnabled(true);
    textAnswer.addListener(this);
    
    // Add and configure the numeric slider
    addAndMakeVisible(numericSlider);
    numericSlider.setRange(1.0, 10.0, 1.0);
    numericSlider.setValue(5.0);
    numericSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    numericSlider.addListener(this);
    
    // Add and configure the duration type combo box
    addAndMakeVisible(durationTypeComboBox);
    durationTypeComboBox.addItem("Seconds", 1);
    durationTypeComboBox.addItem("Measures", 2);
    durationTypeComboBox.setSelectedItemIndex(0);
    durationTypeComboBox.addListener(this);
    
    // Add and configure the next button
    addAndMakeVisible(nextButton);
    nextButton.setButtonText("Next");
    nextButton.addListener(this);
    
    // Add and configure the status label
    addAndMakeVisible(statusLabel);
    statusLabel.setText("Answer the questions to refine your generation", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    
    // Update the layout
    resized();
}

void StreamlineEditor::showNextQuestion()
{
    // Update the question label
    questionLabel.setText(questions[currentQuestionIndex], juce::dontSendNotification);
    
    // Set up the answer component based on the question type
    if (currentQuestionIndex < questionTypes.size())
    {
        int questionType = questionTypes[currentQuestionIndex];
        
        if (questionType == 0) // Multiple choice
        {
            // Multiple choice question
            multipleChoiceOptions.clear();
            multipleChoiceOptions.setVisible(true);
            textAnswer.setVisible(false);
            numericSlider.setVisible(false);
            durationTypeComboBox.setVisible(false);
            
            // Add the options
            int itemId = 1;
            
            for (auto& option : possibleAnswers[currentQuestionIndex])
            {
                multipleChoiceOptions.addItem(option, itemId++);
            }
            
            // Select the first option by default
            multipleChoiceOptions.setSelectedItemIndex(0);
        }
        else if (questionType == 1) // Text input
        {
            // Text input question
            multipleChoiceOptions.setVisible(false);
            textAnswer.setVisible(true);
            numericSlider.setVisible(false);
            durationTypeComboBox.setVisible(false);
            
            textAnswer.setText("", false);
            textAnswer.setTextToShowWhenEmpty("Enter your answer", juce::Colours::grey);
        }
        else if (questionType == 2) // Numeric slider
        {
            // Numeric slider question
            multipleChoiceOptions.setVisible(false);
            textAnswer.setVisible(false);
            numericSlider.setVisible(true);
            durationTypeComboBox.setVisible(false);
            
            // Configure the slider based on the question
            if (questions[currentQuestionIndex].contains("1-10"))
            {
                numericSlider.setRange(1.0, 10.0, 1.0);
                numericSlider.setValue(5.0);
            }
            else
            {
                numericSlider.setRange(1.0, 100.0, 1.0);
                numericSlider.setValue(50.0);
            }
        }
        else if (questionType == 3) // Duration with type selection
        {
            // Duration input with type selection
            multipleChoiceOptions.setVisible(false);
            textAnswer.setVisible(false);
            numericSlider.setVisible(true);
            durationTypeComboBox.setVisible(true);
            
            // Configure the slider for duration
            numericSlider.setRange(1.0, 60.0, 1.0);
            numericSlider.setValue(16.0); // Default to 16 measures or seconds
            
            // Make sure the duration type is set
            if (durationTypeComboBox.getSelectedId() == 0)
                durationTypeComboBox.setSelectedItemIndex(0);
        }
    }
    
    // Update the layout
    resized();
}

void StreamlineEditor::submitAnswers()
{
    // Change state to generating
    currentState = Generating;
    
    // Update UI
    removeAllChildren();
    addAndMakeVisible(statusLabel);
    statusLabel.setText("Generating music...", juce::dontSendNotification);
    resized();
    
    // Send the answers to the processor
    audioProcessor.processFollowUpAnswers(userAnswers);
}

void StreamlineEditor::updateProjectInfoDisplay()
{
    // Get project information from the processor
    double tempo = audioProcessor.getProjectTempo();
    int timeSigNum = audioProcessor.getProjectTimeSigNumerator();
    int timeSigDenom = audioProcessor.getProjectTimeSigDenominator();
    
    // Create a string with the project info
    juce::String infoText = "Project Info:\n";
    infoText += "Tempo: " + juce::String(tempo) + " BPM\n";
    infoText += "Time Signature: " + juce::String(timeSigNum) + "/" + juce::String(timeSigDenom);
    
    // Update the label
    projectInfoLabel.setText(infoText, juce::dontSendNotification);
}
>>>>>>> 732919b (Updated MVP, fixed readme, tweaked AI settings)
