#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <string>

using namespace juce;

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

class TutorialAudioProcessorEditor  :
    public AudioProcessorEditor, public Timer
{
public:
    TutorialAudioProcessorEditor (TutorialAudioProcessor&, AudioProcessorValueTreeState& vts);
    ~TutorialAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    TutorialAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& valueTreeState;

    // envs
    double hostSampleRate = 48000.;
    
    // input gain
    Slider inputGainSlider;
    Label inputGainLabel;
    
    // output gain
    Slider outputGainSlider;
    Label outputGainLabel;
    
    // -- monitor -- //
    Label bufferTimeLabel;
    
    // attachments
    std::unique_ptr<SliderAttachment> inputGainAttachment;
    std::unique_ptr<SliderAttachment> outputGainAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TutorialAudioProcessorEditor)
};
