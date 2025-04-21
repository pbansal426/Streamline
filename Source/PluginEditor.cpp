#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <unistd.h>
StreamlineAIEditor::StreamlineAIEditor(StreamlineAIProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set up the initial UI
    setupInitialUI();
    
    // Set up callbacks for when generation is complete
    audioProcessor.onMidiGenerated = [this](const juce::MidiFile& midi) {
        // Show completion message
        currentState = Complete;
        setupCompletionUI(true); // true for MIDI
    };
    
    audioProcessor.onAudioGenerated = [this](const juce::AudioBuffer<float>& audio) {
        // Show completion message
        currentState = Complete;
        setupCompletionUI(false); // false for Audio
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
    
    // Start the questionnaire immediately
    startQuestionnaire();
}

StreamlineAIEditor::~StreamlineAIEditor()
{
}

void StreamlineAIEditor::paint(juce::Graphics& g)
{
    // Fill the background
    g.fillAll(juce::Colours::darkgrey);
    
    // Add a title
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("Streamline AI", getLocalBounds().reduced(10), juce::Justification::top, true);
    
    // If we're in the completion state, show a message
    if (currentState == Complete)
    {
        g.setFont(16.0f);
        g.setColour(juce::Colours::lightgreen);
        g.drawText("Generation complete!",
                   getLocalBounds().reduced(10).removeFromTop(60).removeFromTop(30),
                   juce::Justification::centred, true);
    }
}

void StreamlineAIEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    
    // Position the components based on the current state
    if (currentState == Initial)
    {
        // Title area
        area.removeFromTop(30);
        
        // Start button
        generateButton.setBounds(area.removeFromTop(30).withSizeKeepingCentre(200, 30));
    }
    else if (currentState == AskingQuestions)
    {
        // Title area
        area.removeFromTop(30);
        
        // Question label - give it more space
        questionLabel.setBounds(area.removeFromTop(50));
        area.removeFromTop(20);
        
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
                textAnswer.setBounds(area.removeFromTop(80)); // More space for text
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
        
        area.removeFromTop(20);
        
        // Next button
        nextButton.setBounds(area.removeFromTop(30).withSizeKeepingCentre(120, 30));
    }
    else if (currentState == Generating)
    {
        // Title area
        area.removeFromTop(30);
        
        // Just show a centered message
        juce::Label* generatingLabel = new juce::Label();
        addAndMakeVisible(generatingLabel);
        generatingLabel->setText("Generating music...", juce::dontSendNotification);
        generatingLabel->setJustificationType(juce::Justification::centred);
        generatingLabel->setBounds(area.removeFromTop(30));
        
        // Delete the label when no longer needed
        juce::Timer::callAfterDelay(100, [this, generatingLabel]() {
            if (generatingLabel != nullptr && isShowing()) {
                removeChildComponent(generatingLabel);
                delete generatingLabel;
            }
        });
    }
    else if (currentState == Complete)
    {
        // Title area
        area.removeFromTop(60); // Extra space for the completion message
        
        // Display the generated content info
        if (resultLabel.isVisible())
        {
            resultLabel.setBounds(area.removeFromTop(40));
            area.removeFromTop(10);
        }
        
        // Response display - give it most of the remaining space
        if (responseDisplay.isVisible())
        {
            responseDisplay.setBounds(area.removeFromTop(200));
            area.removeFromTop(10);
        }
        
        // New Generation button
        //newGenerationButton.setBounds(area.removeFromTop(30).withSizeKeepingCentre(200, 30));
    }
}

void StreamlineAIEditor::buttonClicked(juce::Button* button)
{
    if (button == &generateButton)
    {
        // Start the questionnaire
        startQuestionnaire();
    }
    else if (button == &newGenerationButton)
    {
        // Reset to initial state and start a new generation
        currentState = Initial;
        setupInitialUI();
        startQuestionnaire();
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
        
        // Check if we need to add adaptive questions based on the answer
        if (currentQuestionIndex == 1) // After tempo selection method
        {
            // If they chose to enter a specific BPM, add a numeric question
            if (multipleChoiceOptions.getText() == "Enter a specific BPM")
            {
                questions.add("Enter the tempo in BPM:");
                possibleAnswers.add(juce::StringArray()); // No predefined options for numeric input
                questionTypes.add(2); // Numeric slider
            }
            else
            {
                // If they chose description, add a multiple choice question
                questions.add("Select a tempo description:");
                juce::StringArray tempoDescOptions;
                tempoDescOptions.add("Very Slow (40-60 BPM)");
                tempoDescOptions.add("Slow (60-80 BPM)");
                tempoDescOptions.add("Medium (90-120 BPM)");
                tempoDescOptions.add("Fast (130-160 BPM)");
                tempoDescOptions.add("Very Fast (170+ BPM)");
                possibleAnswers.add(tempoDescOptions);
                questionTypes.add(0); // Multiple choice
            }
            
            // Add time signature question
            questions.add("What time signature would you like?");
            juce::StringArray timeSignatureOptions;
            timeSignatureOptions.add("4/4 (Common time)");
            timeSignatureOptions.add("3/4 (Waltz time)");
            timeSignatureOptions.add("6/8 (Compound duple)");
            timeSignatureOptions.add("5/4 (Quintuple)");
            timeSignatureOptions.add("7/8 (Septuple)");
            timeSignatureOptions.add("12/8 (Compound quadruple)");
            possibleAnswers.add(timeSignatureOptions);
            questionTypes.add(0); // Multiple choice
            
            // Add key signature question
            questions.add("What key would you like the music to be in?");
            juce::StringArray keyOptions;
            keyOptions.add("C Major / A Minor");
            keyOptions.add("G Major / E Minor");
            keyOptions.add("D Major / B Minor");
            keyOptions.add("A Major / F# Minor");
            keyOptions.add("E Major / C# Minor");
            keyOptions.add("B Major / G# Minor");
            keyOptions.add("F# Major / D# Minor");
            keyOptions.add("C# Major / A# Minor");
            keyOptions.add("F Major / D Minor");
            keyOptions.add("Bb Major / G Minor");
            keyOptions.add("Eb Major / C Minor");
            keyOptions.add("Ab Major / F Minor");
            keyOptions.add("Db Major / Bb Minor");
            keyOptions.add("Gb Major / Eb Minor");
            keyOptions.add("Cb Major / Ab Minor");
            possibleAnswers.add(keyOptions);
            questionTypes.add(0); // Multiple choice
            
            // Add instrument choice
            questions.add("What instrument would you like to generate?");
            juce::StringArray instrumentOptions;
            instrumentOptions.add("Piano");
            instrumentOptions.add("Guitar");
            instrumentOptions.add("Bass");
            instrumentOptions.add("Drums");
            instrumentOptions.add("Strings");
            instrumentOptions.add("Brass");
            instrumentOptions.add("Synth");
            instrumentOptions.add("Vocal");
            possibleAnswers.add(instrumentOptions);
            questionTypes.add(0); // Multiple choice
            
            // Add genre
            questions.add("What genre should the music be?");
            juce::StringArray genreOptions;
            genreOptions.add("Rock");
            genreOptions.add("Jazz");
            genreOptions.add("Classical");
            genreOptions.add("Electronic");
            genreOptions.add("Hip Hop");
            genreOptions.add("Pop");
            genreOptions.add("Folk");
            genreOptions.add("R&B");
            possibleAnswers.add(genreOptions);
            questionTypes.add(0); // Multiple choice
            
            // Add style
            questions.add("What specific style within that genre?");
            juce::StringArray styleOptions;
            styleOptions.add("Standard/Traditional");
            styleOptions.add("Fusion");
            styleOptions.add("Experimental");
            styleOptions.add("Minimalist");
            styleOptions.add("Progressive");
            styleOptions.add("Ambient");
            styleOptions.add("Other (specify in additional notes)");
            possibleAnswers.add(styleOptions);
            questionTypes.add(0); // Multiple choice
            
            // Add number of measures
            questions.add("How many measures would you like?");
            possibleAnswers.add(juce::StringArray()); // No predefined options for numeric input
            questionTypes.add(2); // Numeric slider
            
            // Add mood
            questions.add("What's the mood of the piece?");
            juce::StringArray moodOptions;
            moodOptions.add("Happy");
            moodOptions.add("Sad");
            moodOptions.add("Energetic");
            moodOptions.add("Relaxed");
            moodOptions.add("Aggressive");
            moodOptions.add("Mysterious");
            moodOptions.add("Romantic");
            moodOptions.add("Epic");
            possibleAnswers.add(moodOptions);
            questionTypes.add(0); // Multiple choice
            
            // Add rhythm complexity
            questions.add("How complex should the rhythm be? (1-10)");
            possibleAnswers.add(juce::StringArray()); // No predefined options for slider
            questionTypes.add(2); // Numeric slider
            
            // Add melodic complexity
            questions.add("How complex should the melody be? (1-10)");
            possibleAnswers.add(juce::StringArray()); // No predefined options for slider
            questionTypes.add(2); // Numeric slider
            
            // Add additional notes
            questions.add("Any additional notes or specific elements you'd like included?");
            possibleAnswers.add(juce::StringArray()); // No predefined options for text input
            questionTypes.add(1); // Text input
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

void StreamlineAIEditor::textEditorReturnKeyPressed(juce::TextEditor& editor)
{
    if (&editor == &textAnswer && currentState == AskingQuestions)
    {
        // Same as clicking the next button
        buttonClicked(&nextButton);
    }
}

void StreamlineAIEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    // Handle combo box changes if needed
}

void StreamlineAIEditor::sliderValueChanged(juce::Slider* slider)
{
    // Handle slider changes if needed
}

void StreamlineAIEditor::setupInitialUI()
{
    // Remove all existing components
    removeAllChildren();
    
    // Add and configure the generate button
    addAndMakeVisible(generateButton);
    generateButton.setButtonText("Start Music Generation");
    generateButton.addListener(this);
    
    // Update the layout
    resized();
}

void StreamlineAIEditor::setupQuestionsUI()
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
    
    // Update the layout
    resized();
}

// Update the setupCompletionUI method to ensure the response display is properly configured
void StreamlineAIEditor::setupCompletionUI(bool isMidi)
{
    try {
        // Remove all existing components
        removeAllChildren();
        
        // Add result label
        addAndMakeVisible(resultLabel);
        resultLabel.setJustificationType(juce::Justification::centred);
        
        if (isMidi)
        {
            resultLabel.setText("MIDI sequence generated successfully.\nYou can now copy it to your DAW.", juce::dontSendNotification);
        }
        else
        {
            resultLabel.setText("Audio generated successfully.\nYou can now use it in your project.", juce::dontSendNotification);
        }
        addAndMakeVisible(responseDisplay);
        responseDisplay.setMultiLine(true);
        responseDisplay.setReadOnly(true);
        responseDisplay.setScrollbarsShown(true);
        responseDisplay.setCaretVisible(false);
        responseDisplay.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 12.0f, juce::Font::plain));

        // --- begin snippet ---

        juce::String responseText = createFormattedResponseText();

            // 2) Locate your project root (where process.py lives)
            //    For simplicity, hard‑code it here during development:
            static const juce::File projectRoot ("/Users/prathambansal/Projects/Streamline");

            // 3) Write responseText into text.txt beside process.py
            juce::File textFile = projectRoot.getChildFile("text.txt");
            juce::Logger::writeToLog("[DEBUG] Writing to: " + textFile.getFullPathName());
            if (! textFile.replaceWithText(responseText))
            {
                juce::Logger::writeToLog("[ERROR] Failed to write text.txt");
                return;
            }
            juce::Logger::writeToLog("[DEBUG] ✓ Wrote text.txt (" + juce::String(textFile.getSize()) + " bytes)");

            // 4) Build absolute path to your Python script
            juce::File scriptFile = projectRoot.getChildFile("process.py");
            juce::String cmd = "/opt/homebrew/bin/python3 \"" + scriptFile.getFullPathName() + "\"";
            // adjust python path if needed

            juce::Logger::writeToLog("[DEBUG] Running: " + cmd);

            // 5) Launch the Python script and capture its output
            juce::ChildProcess pythonProc;
            if (pythonProc.start(cmd))
            {
                juce::String output;
                while (pythonProc.isRunning())
                {
                    output += pythonProc.readAllProcessOutput();
                    juce::Thread::sleep(10);
                }
                // read any remaining output
                output += pythonProc.readAllProcessOutput();
                juce::Logger::writeToLog("[Python stdout]\n" + output);
            }
            else
            {
                juce::Logger::writeToLog("[ERROR] Failed to start Python script");
            }


        responseDisplay.setText(responseText, false);
        
        juce::Logger::writeToLog("Response text: " + responseText);
      
        addAndMakeVisible(newGenerationButton);
        newGenerationButton.setButtonText("Start New Generation");
        newGenerationButton.addListener(this);
        
        resized();
//        sleep(4);
//        currentState = Initial;
//        setupInitialUI();
//        startQuestionnaire();
        // Update the layout
    }
    catch (const std::exception& e) {
        // Log any exceptions
        juce::Logger::writeToLog("Exception in setupCompletionUI: " + juce::String(e.what()));
    }
}

// New method to create formatted response text directly from the collected answers
juce::String StreamlineAIEditor::createFormattedResponseText()
{
    if (userAnswers.size() == 0)
        return "No responses collected yet.";
    
    juce::String result = "User Responses:\n\n";
    
    // Add the questions and answers
    int questionIndex = 0;
    
    // First two questions are always the same
    if (questionIndex < userAnswers.size()) {
        result += "Output Format: " + userAnswers[questionIndex] + "\n\n";
        questionIndex++;
    }
    
    if (questionIndex < userAnswers.size()) {
        result += "Tempo Specification: " + userAnswers[questionIndex] + "\n\n";
        questionIndex++;
    }
    
    // Handle the dynamic questions based on the tempo specification
    if (userAnswers.size() > 1) {
        if (userAnswers[1] == "Enter a specific BPM" && questionIndex < userAnswers.size()) {
            result += "Specific BPM: " + userAnswers[questionIndex] + "\n\n";
            questionIndex++;
        } else if (questionIndex < userAnswers.size()) {
            result += "Tempo Description: " + userAnswers[questionIndex] + "\n\n";
            questionIndex++;
        }
    }
    
    // Add the remaining standard questions
    if (questionIndex < userAnswers.size()) {
        result += "Time Signature: " + userAnswers[questionIndex] + "\n\n";
        questionIndex++;
    }
    
    if (questionIndex < userAnswers.size()) {
        result += "Key: " + userAnswers[questionIndex] + "\n\n";
        questionIndex++;
    }
    
    if (questionIndex < userAnswers.size()) {
        result += "Instrument: " + userAnswers[questionIndex] + "\n\n";
        questionIndex++;
    }
    
    if (questionIndex < userAnswers.size()) {
        result += "Genre: " + userAnswers[questionIndex] + "\n\n";
        questionIndex++;
    }
    
    if (questionIndex < userAnswers.size()) {
        result += "Style: " + userAnswers[questionIndex] + "\n\n";
        questionIndex++;
    }
    
    if (questionIndex < userAnswers.size()) {
        result += "Measures: " + userAnswers[questionIndex] + "\n\n";
        questionIndex++;
    }
    
    if (questionIndex < userAnswers.size()) {
        result += "Mood: " + userAnswers[questionIndex] + "\n\n";
        questionIndex++;
    }
    
    if (questionIndex < userAnswers.size()) {
        result += "Rhythm Complexity: " + userAnswers[questionIndex] + "/10\n\n";
        questionIndex++;
    }
    
    if (questionIndex < userAnswers.size()) {
        result += "Melodic Complexity: " + userAnswers[questionIndex] + "/10\n\n";
        questionIndex++;
    }
    
    if (questionIndex < userAnswers.size()) {
        result += "Additional Notes: " + userAnswers[questionIndex] + "\n\n";
        questionIndex++;
    }
    
    // Add any remaining answers
    while (questionIndex < userAnswers.size()) {
        result += "Answer " + juce::String(questionIndex + 1) + ": " + userAnswers[questionIndex] + "\n\n";
        questionIndex++;
    }
    
    return result;
}

void StreamlineAIEditor::showNextQuestion()
{
    // Clear the question label first to avoid text overlap
    questionLabel.setText("", juce::dontSendNotification);
    
    // Update the question label with the new question
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
            if (questions[currentQuestionIndex].contains("complexity"))
            {
                numericSlider.setRange(1.0, 10.0, 1.0);
                numericSlider.setValue(5.0);
            }
            else if (questions[currentQuestionIndex].contains("measures"))
            {
                numericSlider.setRange(1.0, 64.0, 1.0);
                numericSlider.setValue(16.0); // Default to 16 measures
            }
            else if (questions[currentQuestionIndex].contains("tempo") ||
                     questions[currentQuestionIndex].contains("BPM"))
            {
                numericSlider.setRange(40.0, 200.0, 1.0);
                numericSlider.setValue(120.0); // Default to 120 BPM
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

void StreamlineAIEditor::submitAnswers()
{
    // Change state to generating
    currentState = Generating;
    
    // Update UI
    removeAllChildren();
    
    // We'll just show a temporary message in the resized() method
    resized();
    
    // Log all answers for debugging
    juce::String allAnswers = "All answers: ";
    for (auto& answer : userAnswers) {
        allAnswers += answer + ", ";
    }
    juce::Logger::writeToLog(allAnswers);
    
    // Send the answers to the processor
    audioProcessor.processFollowUpAnswers(userAnswers);
    
    // Now actually generate the music
    audioProcessor.generateMusic();
}

void StreamlineAIEditor::startQuestionnaire()
{
    // Call the processor to generate the questions
    // We'll pass an empty prompt since we're not using it anymore
    audioProcessor.generateMusicFromPrompt("");
}
