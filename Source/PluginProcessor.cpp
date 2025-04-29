<<<<<<< HEAD
#include "PluginProcessor.h"
#include "PluginEditor.h"

StreamlineAIProcessor::StreamlineAIProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Initialize with default values
    projectTempo = 120.0;
    timeSigNumerator = 4;
    timeSigDenominator = 4;
    
    // Initialize the user responses dictionary
    userResponsesDict = std::make_unique<juce::DynamicObject>();
}

StreamlineAIProcessor::~StreamlineAIProcessor()
{
}

void StreamlineAIProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize buffers and prepare for playback
    generatedAudio.setSize(2, samplesPerBlock);
    generatedAudio.clear();
}

void StreamlineAIProcessor::releaseResources()
{
    // Free resources when plugin is not being used
}

void StreamlineAIProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    
    // If we have generated audio, play it
    if (isPlaying && generatedAudio.getNumSamples() > 0)
    {
        int numSamples = buffer.getNumSamples();
        int numChannels = buffer.getNumChannels();
        
        // Copy from generated audio to output buffer
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* outputData = buffer.getWritePointer(channel);
            const float* inputData = generatedAudio.getReadPointer(channel);
            
            // Copy samples, handling loop points if needed
            for (int sample = 0; sample < numSamples; ++sample)
            {
                if (currentPosition < generatedAudio.getNumSamples())
                {
                    outputData[sample] = inputData[currentPosition];
                }
                else
                {
                    outputData[sample] = 0.0f;
                }
                
                currentPosition++;
                
                // Loop back to beginning if we reach the end
                if (currentPosition >= generatedAudio.getNumSamples())
                {
                    currentPosition = 0;
                }
            }
        }
    }
    
    // Process MIDI messages if we have generated MIDI
    if (isPlaying && midiSequence.getNumEvents() > 0)
    {
        // Add MIDI events that should play during this buffer
        double sampleRate = getSampleRate();
        double samplesPerTick = sampleRate / 960.0; // Assuming 960 PPQN
        
        for (int i = 0; i < midiSequence.getNumEvents(); ++i)
        {
            auto* event = midiSequence.getEventPointer(i);
            int eventPosition = static_cast<int>(event->message.getTimeStamp() * samplesPerTick);
            
            if (eventPosition >= currentPosition && eventPosition < currentPosition + buffer.getNumSamples())
            {
                int offsetInBuffer = eventPosition - currentPosition;
                midiMessages.addEvent(event->message, offsetInBuffer);
            }
        }
    }
}

juce::AudioProcessorEditor* StreamlineAIProcessor::createEditor()
{
    return new StreamlineAIEditor(*this);
}

bool StreamlineAIProcessor::hasEditor() const
{
    return true;
}

const juce::String StreamlineAIProcessor::getName() const
{
    return JucePlugin_Name;
}

int StreamlineAIProcessor::getNumPrograms()
{
    return 1;
}

int StreamlineAIProcessor::getCurrentProgram()
{
    return 0;
}

void StreamlineAIProcessor::setCurrentProgram(int index)
{
}

const juce::String StreamlineAIProcessor::getProgramName(int index)
{
    return {};
}

void StreamlineAIProcessor::changeProgramName(int index, const juce::String& newName)
{
}

void StreamlineAIProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Store plugin state
}

void StreamlineAIProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
}

double StreamlineAIProcessor::getProjectTempo() const
{
    return projectTempo;
}

void StreamlineAIProcessor::setProjectTempo(double tempo)
{
    projectTempo = tempo;
}

int StreamlineAIProcessor::getProjectTimeSigNumerator() const
{
    return timeSigNumerator;
}

void StreamlineAIProcessor::setProjectTimeSigNumerator(int num)
{
    timeSigNumerator = num;
}

int StreamlineAIProcessor::getProjectTimeSigDenominator() const
{
    return timeSigDenominator;
}

void StreamlineAIProcessor::setProjectTimeSigDenominator(int denom)
{
    timeSigDenominator = denom;
}

juce::StringArray StreamlineAIProcessor::getProjectTrackInfo() const
{
    // In a real implementation, you would try to get track info from the host
    // This is a placeholder that returns mock data
    juce::StringArray tracks;
    tracks.add("Track 1: Drums");
    tracks.add("Track 2: Bass");
    tracks.add("Track 3: Piano");
    
    return tracks;
}

void StreamlineAIProcessor::generateMusicFromPrompt(const juce::String& prompt)
{
    // Only generate AI questions based on the prompt
    // The actual music generation will happen after all questions are answered
    generateAIQuestions(prompt);
}

// Add a new method to actually generate the music after all questions are answered
void StreamlineAIProcessor::generateMusic()
{
    // For this example, we'll just create a simple MIDI sequence after a delay
    juce::Timer::callAfterDelay(2000, [this]() {
        // Create a simple MIDI sequence (C major scale)
        midiSequence.clear();
        
        for (int note = 60; note < 72; ++note) {
            // Note on
            juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, note, (juce::uint8) 100);
            noteOn.setTimeStamp(note - 60);
            midiSequence.addEvent(noteOn);
            
            // Note off
            juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, note);
            noteOff.setTimeStamp(note - 59);
            midiSequence.addEvent(noteOff);
        }
        
        // Sort the sequence by timestamp
        midiSequence.sort();
        
        // Add the sequence to the MIDI file
        generatedMidi.clear();
        generatedMidi.addTrack(midiSequence);
        
        // Start playback
        currentPosition = 0;
        isPlaying = true;
        
        // Notify the editor based on output format
        if (outputAsMidi) {
            if (onMidiGenerated)
                onMidiGenerated(generatedMidi);
        } else {
            // In a real implementation, you would render audio here
            // For now, we'll just notify with the same MIDI data
            if (onAudioGenerated)
                onAudioGenerated(generatedAudio);
        }
    });
}

// Update the processFollowUpAnswers method to just store the answers without triggering generation
void StreamlineAIProcessor::processFollowUpAnswers(const juce::StringArray& answers)
{
    try {
        // Make sure we have a valid dictionary object
        if (!userResponsesDict)
            userResponsesDict = std::make_unique<juce::DynamicObject>();
        
        // Clear any existing properties to avoid duplicates
        userResponsesDict->getProperties().clear();
        
        // First answer determines output type
        if (answers.size() > 0) {
            outputAsMidi = (answers[0] == "MIDI");
            userResponsesDict->setProperty("outputFormat", answers[0]);
            juce::Logger::writeToLog("Output format: " + juce::String(outputAsMidi ? "MIDI" : "Audio"));
        }
        
        // Add basic properties
        userResponsesDict->setProperty("timestamp", juce::Time::getCurrentTime().toString(true, true));
        
        // Create a simple dictionary with question numbers as keys
        for (int i = 0; i < answers.size(); ++i) {
            juce::String key = "question_" + juce::String(i + 1);
            userResponsesDict->setProperty(key, answers[i]);
            juce::Logger::writeToLog(key + ": " + answers[i]);
        }
        
        // For dynamically added questions, we need to track which questions were asked
        int currentIndex = 2; // Start after the first two fixed questions
        
        // Check if we have a specific BPM or a tempo description
        if (answers.size() > 1 && answers[1] == "Enter a specific BPM") {
            if (answers.size() > currentIndex) {
                userResponsesDict->setProperty("specificBPM", answers[currentIndex]);
                currentIndex++;
            }
        } else {
            if (answers.size() > currentIndex) {
                userResponsesDict->setProperty("tempoDescription", answers[currentIndex]);
                currentIndex++;
            }
        }
        
        // Add remaining properties if they exist
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("timeSignature", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("key", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("instrument", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("genre", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("style", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("measures", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("mood", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("rhythmComplexity", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("melodicComplexity", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("additionalNotes", answers[currentIndex++]);
        }
        
        // Log the full dictionary for debugging
        juce::String jsonString = juce::JSON::toString(userResponsesDict.get());
        juce::Logger::writeToLog("Full response dictionary: " + jsonString);
    }
    catch (const std::exception& e) {
        // Log any exceptions
        juce::Logger::writeToLog("Exception in processFollowUpAnswers: " + juce::String(e.what()));
    }
}

juce::String StreamlineAIProcessor::getFormattedResponses() const
{
    try {
        if (!userResponsesDict || userResponsesDict->getProperties().size() == 0)
            return "No responses collected yet.";
        
        // Create a more detailed string representation of the dictionary
        juce::String result = "User Responses:\n\n";
        
        // Get all properties
        auto& properties = userResponsesDict->getProperties();
        
        // Add each property to the result string with better formatting
        for (int i = 0; i < properties.size(); ++i) {
            auto name = properties.getName(i).toString();
            auto value = properties.getValueAt(i).toString();
            
            // Skip complex objects and timestamp
            if (value.startsWith("{") || value.startsWith("[") || name == "timestamp")
                continue;
                
            // Format the property name to be more readable
            juce::String displayName = name;
            if (name.startsWith("question_")) {
                // Convert question_1 to Question 1
                displayName = "Question " + name.substring(9);
            } else {
                // Convert camelCase to Title Case with spaces
                displayName = displayName.substring(0, 1).toUpperCase();
                for (int j = 1; j < displayName.length(); ++j) {
                    if (std::isupper(displayName[j])) {
                        displayName = displayName.substring(0, j) + " " + displayName.substring(j);
                        j++;
                    }
                }
            }
            
            result += displayName + ": " + value + "\n\n";
        }
        
        // Add some specific formatted entries for common questions
        if (properties.contains("outputFormat"))
            result += "Output Format: " + properties["outputFormat"].toString() + "\n\n";
            
        if (properties.contains("specificBPM"))
            result += "Tempo: " + properties["specificBPM"].toString() + " BPM\n\n";
        else if (properties.contains("tempoDescription"))
            result += "Tempo: " + properties["tempoDescription"].toString() + "\n\n";
            
        if (properties.contains("timeSignature"))
            result += "Time Signature: " + properties["timeSignature"].toString() + "\n\n";
            
        if (properties.contains("key"))
            result += "Key: " + properties["key"].toString() + "\n\n";
            
        if (properties.contains("instrument"))
            result += "Instrument: " + properties["instrument"].toString() + "\n\n";
            
        if (properties.contains("genre"))
            result += "Genre: " + properties["genre"].toString() + "\n\n";
            
        if (properties.contains("style"))
            result += "Style: " + properties["style"].toString() + "\n\n";
            
        if (properties.contains("measures"))
            result += "Measures: " + properties["measures"].toString() + "\n\n";
            
        if (properties.contains("mood"))
            result += "Mood: " + properties["mood"].toString() + "\n\n";
            
        if (properties.contains("rhythmComplexity"))
            result += "Rhythm Complexity: " + properties["rhythmComplexity"].toString() + "/10\n\n";
            
        if (properties.contains("melodicComplexity"))
            result += "Melodic Complexity: " + properties["melodicComplexity"].toString() + "/10\n\n";
            
        if (properties.contains("additionalNotes") && !properties["additionalNotes"].toString().isEmpty())
            result += "Additional Notes: " + properties["additionalNotes"].toString() + "\n\n";
        
        return result;
    }
    catch (const std::exception& e) {
        // Return error message if something goes wrong
        return "Error formatting responses: " + juce::String(e.what());
    }
}

void StreamlineAIProcessor::generateAIQuestions(const juce::String& initialPrompt)
{
    // Initialize arrays for questions
    juce::StringArray questions;
    juce::Array<juce::StringArray> possibleAnswers;
    juce::Array<int> questionTypes;
    
    // Question 1: Output format (MIDI or Audio)
    questions.add("Do you want MIDI or Audio output?");
    juce::StringArray formatOptions;
    formatOptions.add("MIDI");
    formatOptions.add("Audio");
    possibleAnswers.add(formatOptions);
    questionTypes.add(0); // Multiple choice
    
    // Question 2: Tempo
    questions.add("How would you like to specify the tempo?");
    juce::StringArray tempoOptions;
    tempoOptions.add("Enter a specific BPM");
    tempoOptions.add("Use a tempo description");
    possibleAnswers.add(tempoOptions);
    questionTypes.add(0); // Multiple choice
    
    // We'll add the rest of the questions dynamically based on user responses
    // The editor will handle this by checking the answers and adding new questions as needed
    
    // Notify the editor with the initial questions
    if (onQuestionsGenerated)
        onQuestionsGenerated(questions, possibleAnswers, questionTypes);
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StreamlineAIProcessor();
}
=======
#include "PluginProcessor.h"
#include "PluginEditor.h"

StreamlineAIProcessor::StreamlineAIProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Initialize with default values
    projectTempo = 120.0;
    timeSigNumerator = 4;
    timeSigDenominator = 4;
    
    // Initialize the user responses dictionary
    userResponsesDict = std::make_unique<juce::DynamicObject>();
}

StreamlineAIProcessor::~StreamlineAIProcessor()
{
}

void StreamlineAIProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize buffers and prepare for playback
    generatedAudio.setSize(2, samplesPerBlock);
    generatedAudio.clear();
}

void StreamlineAIProcessor::releaseResources()
{
    // Free resources when plugin is not being used
}

void StreamlineAIProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    
    // If we have generated audio, play it
    if (isPlaying && generatedAudio.getNumSamples() > 0)
    {
        int numSamples = buffer.getNumSamples();
        int numChannels = buffer.getNumChannels();
        
        // Copy from generated audio to output buffer
        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* outputData = buffer.getWritePointer(channel);
            const float* inputData = generatedAudio.getReadPointer(channel);
            
            // Copy samples, handling loop points if needed
            for (int sample = 0; sample < numSamples; ++sample)
            {
                if (currentPosition < generatedAudio.getNumSamples())
                {
                    outputData[sample] = inputData[currentPosition];
                }
                else
                {
                    outputData[sample] = 0.0f;
                }
                
                currentPosition++;
                
                // Loop back to beginning if we reach the end
                if (currentPosition >= generatedAudio.getNumSamples())
                {
                    currentPosition = 0;
                }
            }
        }
    }
    
    // Process MIDI messages if we have generated MIDI
    if (isPlaying && midiSequence.getNumEvents() > 0)
    {
        // Add MIDI events that should play during this buffer
        double sampleRate = getSampleRate();
        double samplesPerTick = sampleRate / 960.0; // Assuming 960 PPQN
        
        for (int i = 0; i < midiSequence.getNumEvents(); ++i)
        {
            auto* event = midiSequence.getEventPointer(i);
            int eventPosition = static_cast<int>(event->message.getTimeStamp() * samplesPerTick);
            
            if (eventPosition >= currentPosition && eventPosition < currentPosition + buffer.getNumSamples())
            {
                int offsetInBuffer = eventPosition - currentPosition;
                midiMessages.addEvent(event->message, offsetInBuffer);
            }
        }
    }
}

juce::AudioProcessorEditor* StreamlineAIProcessor::createEditor()
{
    return new StreamlineAIEditor(*this);
}

bool StreamlineAIProcessor::hasEditor() const
{
    return true;
}

const juce::String StreamlineAIProcessor::getName() const
{
    return JucePlugin_Name;
}

int StreamlineAIProcessor::getNumPrograms()
{
    return 1;
}

int StreamlineAIProcessor::getCurrentProgram()
{
    return 0;
}

void StreamlineAIProcessor::setCurrentProgram(int index)
{
}

const juce::String StreamlineAIProcessor::getProgramName(int index)
{
    return {};
}

void StreamlineAIProcessor::changeProgramName(int index, const juce::String& newName)
{
}

void StreamlineAIProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Store plugin state
}

void StreamlineAIProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
}

double StreamlineAIProcessor::getProjectTempo() const
{
    return projectTempo;
}

void StreamlineAIProcessor::setProjectTempo(double tempo)
{
    projectTempo = tempo;
}

int StreamlineAIProcessor::getProjectTimeSigNumerator() const
{
    return timeSigNumerator;
}

void StreamlineAIProcessor::setProjectTimeSigNumerator(int num)
{
    timeSigNumerator = num;
}

int StreamlineAIProcessor::getProjectTimeSigDenominator() const
{
    return timeSigDenominator;
}

void StreamlineAIProcessor::setProjectTimeSigDenominator(int denom)
{
    timeSigDenominator = denom;
}

juce::StringArray StreamlineAIProcessor::getProjectTrackInfo() const
{
    // In a real implementation, you would try to get track info from the host
    // This is a placeholder that returns mock data
    juce::StringArray tracks;
    tracks.add("Track 1: Drums");
    tracks.add("Track 2: Bass");
    tracks.add("Track 3: Piano");
    
    return tracks;
}

void StreamlineAIProcessor::generateMusicFromPrompt(const juce::String& prompt)
{
    // Only generate AI questions based on the prompt
    // The actual music generation will happen after all questions are answered
    generateAIQuestions(prompt);
}

// Add a new method to actually generate the music after all questions are answered
void StreamlineAIProcessor::generateMusic()
{
    // For this example, we'll just create a simple MIDI sequence after a delay
    juce::Timer::callAfterDelay(2000, [this]() {
        // Create a simple MIDI sequence (C major scale)
        midiSequence.clear();
        
        for (int note = 60; note < 72; ++note) {
            // Note on
            juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, note, (juce::uint8) 100);
            noteOn.setTimeStamp(note - 60);
            midiSequence.addEvent(noteOn);
            
            // Note off
            juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, note);
            noteOff.setTimeStamp(note - 59);
            midiSequence.addEvent(noteOff);
        }
        
        // Sort the sequence by timestamp
        midiSequence.sort();
        
        // Add the sequence to the MIDI file
        generatedMidi.clear();
        generatedMidi.addTrack(midiSequence);
        
        // Start playback
        currentPosition = 0;
        isPlaying = true;
        
        // Notify the editor based on output format
        if (outputAsMidi) {
            if (onMidiGenerated)
                onMidiGenerated(generatedMidi);
        } else {
            // In a real implementation, you would render audio here
            // For now, we'll just notify with the same MIDI data
            if (onAudioGenerated)
                onAudioGenerated(generatedAudio);
        }
    });
}

// Update the processFollowUpAnswers method to just store the answers without triggering generation
void StreamlineAIProcessor::processFollowUpAnswers(const juce::StringArray& answers)
{
    try {
        // Make sure we have a valid dictionary object
        if (!userResponsesDict)
            userResponsesDict = std::make_unique<juce::DynamicObject>();
        
        // Clear any existing properties to avoid duplicates
        userResponsesDict->getProperties().clear();
        
        // First answer determines output type
        if (answers.size() > 0) {
            outputAsMidi = (answers[0] == "MIDI");
            userResponsesDict->setProperty("outputFormat", answers[0]);
            juce::Logger::writeToLog("Output format: " + juce::String(outputAsMidi ? "MIDI" : "Audio"));
        }
        
        // Add basic properties
        userResponsesDict->setProperty("timestamp", juce::Time::getCurrentTime().toString(true, true));
        
        // Create a simple dictionary with question numbers as keys
        for (int i = 0; i < answers.size(); ++i) {
            juce::String key = "question_" + juce::String(i + 1);
            userResponsesDict->setProperty(key, answers[i]);
            juce::Logger::writeToLog(key + ": " + answers[i]);
        }
        
        // For dynamically added questions, we need to track which questions were asked
        int currentIndex = 2; // Start after the first two fixed questions
        
        // Check if we have a specific BPM or a tempo description
        if (answers.size() > 1 && answers[1] == "Enter a specific BPM") {
            if (answers.size() > currentIndex) {
                userResponsesDict->setProperty("specificBPM", answers[currentIndex]);
                currentIndex++;
            }
        } else {
            if (answers.size() > currentIndex) {
                userResponsesDict->setProperty("tempoDescription", answers[currentIndex]);
                currentIndex++;
            }
        }
        
        // Add remaining properties if they exist
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("timeSignature", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("key", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("instrument", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("genre", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("style", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("measures", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("mood", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("rhythmComplexity", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("melodicComplexity", answers[currentIndex++]);
        }
        if (answers.size() > currentIndex) {
            userResponsesDict->setProperty("additionalNotes", answers[currentIndex++]);
        }
        
        // Log the full dictionary for debugging
        juce::String jsonString = juce::JSON::toString(userResponsesDict.get());
        juce::Logger::writeToLog("Full response dictionary: " + jsonString);
    }
    catch (const std::exception& e) {
        // Log any exceptions
        juce::Logger::writeToLog("Exception in processFollowUpAnswers: " + juce::String(e.what()));
    }
}

juce::String StreamlineAIProcessor::getFormattedResponses() const
{
    try {
        if (!userResponsesDict || userResponsesDict->getProperties().size() == 0)
            return "No responses collected yet.";
        
        // Create a more detailed string representation of the dictionary
        juce::String result = "User Responses:\n\n";
        
        // Get all properties
        auto& properties = userResponsesDict->getProperties();
        
        // Add each property to the result string with better formatting
        for (int i = 0; i < properties.size(); ++i) {
            auto name = properties.getName(i).toString();
            auto value = properties.getValueAt(i).toString();
            
            // Skip complex objects and timestamp
            if (value.startsWith("{") || value.startsWith("[") || name == "timestamp")
                continue;
                
            // Format the property name to be more readable
            juce::String displayName = name;
            if (name.startsWith("question_")) {
                // Convert question_1 to Question 1
                displayName = "Question " + name.substring(9);
            } else {
                // Convert camelCase to Title Case with spaces
                displayName = displayName.substring(0, 1).toUpperCase();
                for (int j = 1; j < displayName.length(); ++j) {
                    if (std::isupper(displayName[j])) {
                        displayName = displayName.substring(0, j) + " " + displayName.substring(j);
                        j++;
                    }
                }
            }
            
            result += displayName + ": " + value + "\n\n";
        }
        
        // Add some specific formatted entries for common questions
        if (properties.contains("outputFormat"))
            result += "Output Format: " + properties["outputFormat"].toString() + "\n\n";
            
        if (properties.contains("specificBPM"))
            result += "Tempo: " + properties["specificBPM"].toString() + " BPM\n\n";
        else if (properties.contains("tempoDescription"))
            result += "Tempo: " + properties["tempoDescription"].toString() + "\n\n";
            
        if (properties.contains("timeSignature"))
            result += "Time Signature: " + properties["timeSignature"].toString() + "\n\n";
            
        if (properties.contains("key"))
            result += "Key: " + properties["key"].toString() + "\n\n";
            
        if (properties.contains("instrument"))
            result += "Instrument: " + properties["instrument"].toString() + "\n\n";
            
        if (properties.contains("genre"))
            result += "Genre: " + properties["genre"].toString() + "\n\n";
            
        if (properties.contains("style"))
            result += "Style: " + properties["style"].toString() + "\n\n";
            
        if (properties.contains("measures"))
            result += "Measures: " + properties["measures"].toString() + "\n\n";
            
        if (properties.contains("mood"))
            result += "Mood: " + properties["mood"].toString() + "\n\n";
            
        if (properties.contains("rhythmComplexity"))
            result += "Rhythm Complexity: " + properties["rhythmComplexity"].toString() + "/10\n\n";
            
        if (properties.contains("melodicComplexity"))
            result += "Melodic Complexity: " + properties["melodicComplexity"].toString() + "/10\n\n";
            
        if (properties.contains("additionalNotes") && !properties["additionalNotes"].toString().isEmpty())
            result += "Additional Notes: " + properties["additionalNotes"].toString() + "\n\n";
        
        return result;
    }
    catch (const std::exception& e) {
        // Return error message if something goes wrong
        return "Error formatting responses: " + juce::String(e.what());
    }
}

void StreamlineAIProcessor::generateAIQuestions(const juce::String& initialPrompt)
{
    // Initialize arrays for questions
    juce::StringArray questions;
    juce::Array<juce::StringArray> possibleAnswers;
    juce::Array<int> questionTypes;
    
    // Question 1: Output format (MIDI or Audio)
    questions.add("Do you want MIDI or Audio output?");
    juce::StringArray formatOptions;
    formatOptions.add("MIDI");
    formatOptions.add("Audio");
    possibleAnswers.add(formatOptions);
    questionTypes.add(0); // Multiple choice
    
    // Question 2: Tempo
    questions.add("How would you like to specify the tempo?");
    juce::StringArray tempoOptions;
    tempoOptions.add("Enter a specific BPM");
    tempoOptions.add("Use a tempo description");
    possibleAnswers.add(tempoOptions);
    questionTypes.add(0); // Multiple choice
    
    // We'll add the rest of the questions dynamically based on user responses
    // The editor will handle this by checking the answers and adding new questions as needed
    
    // Notify the editor with the initial questions
    if (onQuestionsGenerated)
        onQuestionsGenerated(questions, possibleAnswers, questionTypes);
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StreamlineAIProcessor();
}
>>>>>>> 732919b (Updated MVP, fixed readme, tweaked AI settings)
