/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;



class TutorialAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TutorialAudioProcessor();
    ~TutorialAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override                      { return true; };
    
    //==============================================================================
    const String getName() const override                { return JucePlugin_Name; };
    bool acceptsMidi() const override                    { return false; };
    bool producesMidi() const override                   { return false; };
    bool isMidiEffect() const override                   { return false; };
    double getTailLengthSeconds() const override         { return 0.0; };

    //==============================================================================
    int getNumPrograms() override                        { return 1; };
    int getCurrentProgram() override                     { return 0; };
    void setCurrentProgram (int index) override          { };
    const String getProgramName (int index) override     { return {}; };
    void changeProgramName (int index, const String& newName) override {};

    //==============================================================================
    float getInputGain();
    float getOutputGain();
    void ApplyInputGain(AudioBuffer<float>& buffer);
    void ApplyOutputGain(AudioBuffer<float>& buffer);
    
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void releaseResources() override {};

    #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    #endif
    
    // statistics: RTF
    void updateRecord(float newValue);
    float bufferTimeRecords[100] = {0};
    float bufferTimeFull = 0.0;
    int recordIndex = 0;
    
private:
    //==============================================================================
    AudioProcessorValueTreeState parameters;
    
    // DAW env
    int previousNumSamples = 0;
    int numInputChannels = 2;
    int numOutputChannels = 2;
    double hostSampleRate = 48000.;
    
    // gains
    float previousInputGain;
    float previousOutputGain;
   
    // parameters
    std::atomic<float>* inputGainParameter  = nullptr;
    std::atomic<float>* outputGainParameter = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TutorialAudioProcessor)
};
