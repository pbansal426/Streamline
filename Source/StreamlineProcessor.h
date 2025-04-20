#pragma once

#include <JuceHeader.h>

class StreamlineProcessor : public juce::AudioProcessor
{
public:
    StreamlineProcessor();
    ~StreamlineProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // AI generation methods
    void generateMusicFromPrompt(const juce::String& prompt);
    void processFollowUpAnswers(const juce::StringArray& answers);
    
    // Project info methods
    double getProjectTempo() const;
    int getProjectTimeSigNumerator() const;
    int getProjectTimeSigDenominator() const;
    juce::StringArray getProjectTrackInfo() const;
    
    // Callback for when generation is complete
    std::function<void(const juce::MidiFile&)> onMidiGenerated;
    std::function<void(const juce::AudioBuffer<float>&)> onAudioGenerated;
    
    // Callback for when AI generates follow-up questions
    std::function<void(const juce::StringArray& questions,
                       const juce::Array<juce::StringArray>& possibleAnswers,
                       const juce::Array<int>& questionTypes)> onQuestionsGenerated;

private:
    // HTTP client for API calls
    std::unique_ptr<juce::URL::DownloadTask> downloadTask;
    
    // Buffer for generated audio
    juce::AudioBuffer<float> generatedAudio;
    
    // MIDI sequence for generated music
    juce::MidiFile generatedMidi;
    juce::MidiMessageSequence midiSequence;
    
    // Playback position
    int currentPosition = 0;
    bool isPlaying = false;
    
    // Project information
    double projectTempo = 120.0;
    int timeSigNumerator = 4;
    int timeSigDenominator = 4;
    
    // Mock function to generate AI questions (will be replaced with actual AI)
    void generateAIQuestions(const juce::String& initialPrompt);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StreamlineProcessor)
};

