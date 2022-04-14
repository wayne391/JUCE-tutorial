/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TutorialAudioProcessor::TutorialAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::create7point1point4(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::create7point1point4(), true)
                     #endif
                       ),
#endif
parameters(*this, nullptr, juce::Identifier("APVT"),
       {
           
           std::make_unique<AudioParameterFloat> (
                   "Input Gain",      // parameter ID
                   "Input Gain",      // parameter name
                   NormalisableRange<float>(
                       -30.0f,
                       30.0f,
                       0.01f),         // min, max,step
                    0.0f),
           std::make_unique<AudioParameterFloat> (
                   "Output Gain",      // parameter ID
                   "Output Gain",      // parameter name
                   NormalisableRange<float>(
                       -30.0f,
                       30.0f,
                       0.01f),         // min, max,step
                    0.0f),        // default value
       })

{
    // knob values
    inputGainParameter  = parameters.getRawParameterValue ("Input Gain");
    outputGainParameter = parameters.getRawParameterValue ("Output Gain");
    
    // gains
    previousOutputGain = *outputGainParameter;
    previousInputGain  = *inputGainParameter ;
}

TutorialAudioProcessor::~TutorialAudioProcessor()
{
}

//==============================================================================
void TutorialAudioProcessor::prepareToPlay (double sampleRate, int numSamples)
{
    // set host sampling env
    numInputChannels = getTotalNumInputChannels();
    numOutputChannels = getTotalNumOutputChannels();
    hostSampleRate = sampleRate;
    previousNumSamples = numSamples;
    bufferTimeFull = numSamples / sampleRate;
     
    std::cout << "========================" << std::endl;
    std::cout << "            sampleRate: " << sampleRate << std::endl;
    std::cout << "       samplesPerBlock: " << numSamples << std::endl;
    std::cout << " totalNumInputChannels: " << numInputChannels << std::endl;
    std::cout << "========================" << std::endl;

    // -- clear caches -- //
    // clear gain cache
    previousOutputGain = *outputGainParameter + 0.0;
    previousInputGain  = *inputGainParameter  + 0.0;
    
    // clear bufferTimeRecords
    memset(bufferTimeRecords, 0, sizeof(bufferTimeRecords));
    recordIndex = 0;
}


void TutorialAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    int numSamples = buffer.getNumSamples();
    auto begin = std::chrono::high_resolution_clock::now();
    
    // Main DSP procssing block
    if (numSamples > 0)
    {
        
        // We need reseting here beacuse DAWs usually sent testing signal after preparetoplay()
        if (previousNumSamples != numSamples)
        {
            prepareToPlay(hostSampleRate, numSamples); 
        }
        
        // ===== Start Coding ===== //
        // some DSP stuff
        ApplyInputGain(buffer);
        // ...
        // ...
        // dsp functions
        // ...
        ApplyOutputGain(buffer);
        // ===== End Coding ===== //
        
    }
    else
    {
        return; //avoid exception when DAW sents zero length audio buffer
    }
    
    // update RTF
    auto end = std::chrono::high_resolution_clock::now();
    float duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() / 1e9;
    float bufferTime = duration / bufferTimeFull;
    updateRecord(bufferTime);
}



/*
==================================================================================================
 State Block
==================================================================================================
 */

float TutorialAudioProcessor::getInputGain()
{
    return powf(10, (*inputGainParameter) / 20.0);
}

float TutorialAudioProcessor::getOutputGain()
{
    return powf(10, (*outputGainParameter) / 20.0);
}

void TutorialAudioProcessor::ApplyInputGain(AudioBuffer<float>& buffer)
{
    auto currentInputGain = getInputGain();
    if (currentInputGain == previousInputGain)
    {
        buffer.applyGain (currentInputGain);
    }
    else
    {
        auto numSamples = buffer.getNumSamples();
        buffer.applyGainRamp (0, numSamples, previousInputGain, currentInputGain);
        previousInputGain = currentInputGain;
    }
}

void TutorialAudioProcessor::ApplyOutputGain(AudioBuffer<float>& buffer)
{
   
    auto currentOutputGain = getOutputGain();
    if (currentOutputGain == previousOutputGain)
    {
        buffer.applyGain (currentOutputGain);
    }
    else
    {
        auto numSamples = buffer.getNumSamples();
        buffer.applyGainRamp (0, numSamples, previousOutputGain, currentOutputGain);
        previousOutputGain = currentOutputGain;
    }
}



/*
==================================================================================================
 State Block
==================================================================================================
 */
void TutorialAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void TutorialAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}


/*
==================================================================================================
 Misc
==================================================================================================
 */

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TutorialAudioProcessor();
}

juce::AudioProcessorEditor* TutorialAudioProcessor::createEditor()
{
    return new TutorialAudioProcessorEditor (*this, parameters);
}

bool TutorialAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  
    // Ensure # input channel == #output channel
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    return true;
}


/*
==================================================================================================
 Stastics
==================================================================================================
 */


void TutorialAudioProcessor::updateRecord(float newValue)
{
    bufferTimeRecords[recordIndex] = newValue;
    ++recordIndex;
    recordIndex %= 100;
}
