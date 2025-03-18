// PluginProcessor.cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <thread> // For creating a separate thread for network operations
#include <iostream>
#ifdef JUCE_WINDOWS
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
void close_socket(int sock) {
    closesocket(sock);
}
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
void close_socket(int sock) {
    close(sock);
}
#endif

StreamlineAIAudioProcessor::StreamlineAIAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
                        #if ! JucePlugin_IsMidiEffect
                         #if ! JucePlugin_IsSynth
                            .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                         #endif
                            .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                        #endif
                                         )
#endif
{
    // You can add any initialisation code here if needed.
}

StreamlineAIAudioProcessor::~StreamlineAIAudioProcessor()
{
    // Add any cleanup code here.
}

void StreamlineAIAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void StreamlineAIAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc..
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StreamlineAIAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


//==============================================================================
bool StreamlineAIAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* StreamlineAIAudioProcessor::createEditor()
{
    return new StreamlineAIAudioProcessorEditor (*this);
}

//==============================================================================
void StreamlineAIAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void StreamlineAIAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
const juce::String StreamlineAIAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StreamlineAIAudioProcessor::acceptsMidi() const
{
  #if JucePlugin_WantsMidiInput
    return true;
  #else
    return false;
  #endif
}

bool StreamlineAIAudioProcessor::producesMidi() const
{
  #if JucePlugin_ProducesMidiOutput
    return true;
  #else
    return false;
  #endif
}

bool StreamlineAIAudioProcessor::isMidiEffect() const
{
  #if JucePlugin_IsMidiEffect
    return true;
  #else
    return false;
  #endif
}

double StreamlineAIAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================
int StreamlineAIAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int StreamlineAIAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StreamlineAIAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String StreamlineAIAudioProcessor::getProgramName (int index)
{
    return {};
}

void StreamlineAIAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void StreamlineAIAudioProcessor::processPrompt (const juce::String& prompt)
{
    juce::Logger::outputDebugString ("User entered prompt: " + prompt);

    // --- Placeholder for sending to AI model ---
    // Create a separate thread to avoid blocking the audio thread
    std::thread([prompt]() {
        // --- Dummy socket communication for now ---
        int sock = 0;
        struct sockaddr_in serv_addr;
        const char* message = prompt.toStdString().c_str();
        const int port = 12345; // Choose a port

#ifdef JUCE_WINDOWS
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            juce::Logger::outputDebugString("WSAStartup failed");
            return;
        }
#endif

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            juce::Logger::outputDebugString("Socket creation error");
            return;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        // Convert IPv4 or IPv6 addresses from text to binary form
        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
            juce::Logger::outputDebugString("Invalid address/ Address not supported");
            close_socket(sock);
            return;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            juce::Logger::outputDebugString("Connection Failed");
            close_socket(sock);
            return;
        }
        send(sock, message, strlen(message), 0 );
        juce::Logger::outputDebugString("Prompt sent to Python server");

        // --- Placeholder for receiving audio data ---
        // ... code to receive the generated audio data and store it ...

        close_socket(sock);
#ifdef JUCE_WINDOWS
        WSACleanup();
#endif
    }).detach();
}

void StreamlineAIAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        // --- Placeholder for outputting generated audio ---
        // For now, we'll just leave the audio buffer as is (silence).
        // Later, you'll copy the received audio data into this buffer.
    }
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StreamlineAIAudioProcessor();
}
