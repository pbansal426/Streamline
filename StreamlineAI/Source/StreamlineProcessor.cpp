#include "StreamlineProcessor.h"
#include "StreamlineEditor.h"

StreamlineProcessor::StreamlineProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Try to get host information
    if (auto* playHead = getPlayHead())
    {
        juce::AudioPlayHead::CurrentPositionInfo posInfo;
        if (playHead->getCurrentPosition(posInfo))
        {
            projectTempo = posInfo.bpm;
            timeSigNumerator = posInfo.timeSigNumerator;
            timeSigDenominator = posInfo.timeSigDenominator;
        }
    }
}

StreamlineProcessor::~StreamlineProcessor()
{
}

void StreamlineProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize buffers and prepare for playback
    generatedAudio.setSize(2, samplesPerBlock);
    generatedAudio.clear();
    
    // Try to get updated host information
    if (auto* playHead = getPlayHead())
    {
        juce::AudioPlayHead::CurrentPositionInfo posInfo;
        if (playHead->getCurrentPosition(posInfo))
        {
            projectTempo = posInfo.bpm;
            timeSigNumerator = posInfo.timeSigNumerator;
            timeSigDenominator = posInfo.timeSigDenominator;
        }
    }
}

void StreamlineProcessor::releaseResources()
{
    // Free resources when plugin is not being used
}

void StreamlineProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    
    // Update project tempo if it changes
    if (auto* playHead = getPlayHead())
    {
        juce::AudioPlayHead::CurrentPositionInfo posInfo;
        if (playHead->getCurrentPosition(posInfo))
        {
            projectTempo = posInfo.bpm;
            timeSigNumerator = posInfo.timeSigNumerator;
            timeSigDenominator = posInfo.timeSigDenominator;
        }
    }
}

juce::AudioProcessorEditor* StreamlineProcessor::createEditor()
{
    return new StreamlineEditor(*this);
}

bool StreamlineProcessor::hasEditor() const
{
    return true;
}

const juce::String StreamlineProcessor::getName() const
{
    return JucePlugin_Name;
}

int StreamlineProcessor::getNumPrograms()
{
    return 1;
}

int StreamlineProcessor::getCurrentProgram()
{
    return 0;
}

void StreamlineProcessor::setCurrentProgram(int index)
{
}

const juce::String StreamlineProcessor::getProgramName(int index)
{
    return {};
}

void StreamlineProcessor::changeProgramName(int index, const juce::String& newName)
{
}

void StreamlineProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Store plugin state
}

void StreamlineProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
}

double StreamlineProcessor::getProjectTempo() const
{
    return projectTempo;
}

int StreamlineProcessor::getProjectTimeSigNumerator() const
{
    return timeSigNumerator;
}

int StreamlineProcessor::getProjectTimeSigDenominator() const
{
    return timeSigDenominator;
}

juce::StringArray StreamlineProcessor::getProjectTrackInfo() const
{
    // In a real implementation, you would try to get track info from the host
    // This is a placeholder that returns mock data
    juce::StringArray tracks;
    tracks.add("Track 1: Drums");
    tracks.add("Track 2: Bass");
    tracks.add("Track 3: Piano");
    
    return tracks;
}

void StreamlineProcessor::generateMusicFromPrompt(const juce::String& prompt)
{
    // First, generate AI questions based on the prompt
    generateAIQuestions(prompt);
    
    // This is where you would call your AI music generation API
    // For now, we'll simulate the API call with a delay
    
    // Create a JSON object with the prompt
    juce::var jsonData = new juce::DynamicObject();
    jsonData.getDynamicObject()->setProperty("prompt", prompt);
    
    // Convert to JSON string
    juce::String jsonString = juce::JSON::toString(jsonData);
    
    // In a real implementation, you would make an HTTP request to your AI service
    // For example:
    /*
    juce::URL url("https://your-ai-music-api.com/generate");
    
    downloadTask = url.downloadToMemoryAsync(
        juce::URL::DownloadTaskOptions()
            .withExtraHeaders("Content-Type: application/json")
            .withConnectionTimeout(30000)
            .withRequestType("POST")
            .withRequestData(jsonString.toRawUTF8(), jsonString.getNumBytesAsUTF8()),
        [this](const juce::MemoryBlock& data, bool success) {
            if (success) {
                // Parse the response and generate audio/MIDI
                juce::String response(data.getData(), data.getSize());
                juce::var jsonResponse = juce::JSON::parse(response);
                
                // Process the response...
                
                // Notify the editor
                if (onMidiGenerated)
                    onMidiGenerated(generatedMidi);
                
                if (onAudioGenerated)
                    onAudioGenerated(generatedAudio);
            }
        }
    );
    */
    
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
        
        // Notify the editor
        if (onMidiGenerated)
            onMidiGenerated(generatedMidi);
    });
}

void StreamlineProcessor::processFollowUpAnswers(const juce::StringArray& answers)
{
    // Process the follow-up answers to refine the generation
    // This would typically be sent along with the original prompt to the AI service
    
    // For now, we'll just log the answers
    juce::String answersStr = answers.joinIntoString(", ");
    juce::Logger::writeToLog("Follow-up answers: " + answersStr);
    
    // In a real implementation, you would include these answers in your API call
}

void StreamlineProcessor::generateAIQuestions(const juce::String& initialPrompt)
{
    // This is a mock function that simulates AI generating questions
    // In a real implementation, this would call the AI service to get dynamic questions
    
    // For now, we'll create some sample questions based on the initial prompt
    juce::StringArray questions;
    juce::Array<juce::StringArray> possibleAnswers;
    juce::Array<int> questionTypes;
    
    // Question 1: Genre (multiple choice)
    questions.add("What genre should the music be?");
    juce::StringArray genreOptions;
    genreOptions.add("Rock");
    genreOptions.add("Jazz");
    genreOptions.add("Classical");
    genreOptions.add("Electronic");
    genreOptions.add("Hip Hop");
    possibleAnswers.add(genreOptions);
    questionTypes.add(0); // Multiple choice
    
    // Question 2: Tempo (if not detected from project)
    if (projectTempo <= 0)
    {
        questions.add("What tempo (BPM) would you like?");
        juce::StringArray tempoOptions;
        tempoOptions.add("Slow (60-80 BPM)");
        tempoOptions.add("Medium (90-120 BPM)");
        tempoOptions.add("Fast (130-160 BPM)");
        tempoOptions.add("Very Fast (170+ BPM)");
        possibleAnswers.add(tempoOptions);
        questionTypes.add(0); // Multiple choice
    }
    
    // Question 3: Mood (multiple choice)
    questions.add("What's the mood of the piece?");
    juce::StringArray moodOptions;
    moodOptions.add("Happy");
    moodOptions.add("Sad");
    moodOptions.add("Energetic");
    moodOptions.add("Relaxed");
    moodOptions.add("Aggressive");
    possibleAnswers.add(moodOptions);
    questionTypes.add(0); // Multiple choice
    
    // Question 4: Duration (numeric with type selection)
    questions.add("How long should the piece be?");
    possibleAnswers.add(juce::StringArray()); // No predefined options for numeric input
    questionTypes.add(3); // Duration type (with seconds/measures selector)
    
    // Question 5: Complexity (numeric slider)
    questions.add("How complex should the music be? (1-10)");
    possibleAnswers.add(juce::StringArray()); // No predefined options for slider
    questionTypes.add(2); // Numeric slider
    
    // Question 6: Additional notes (text input)
    questions.add("Any additional notes or specific elements you'd like included?");
    possibleAnswers.add(juce::StringArray()); // No predefined options for text input
    questionTypes.add(1); // Text input
    
    // Notify the editor with the generated questions
    if (onQuestionsGenerated)
        onQuestionsGenerated(questions, possibleAnswers, questionTypes);
}

