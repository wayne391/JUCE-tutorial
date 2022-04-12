/*
  ==============================================================================
    This file was auto-generated!
    It contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TutorialAudioProcessorEditor::TutorialAudioProcessorEditor (TutorialAudioProcessor& p,  AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState (vts)
{

    // input gain
    addAndMakeVisible(inputGainSlider);
    inputGainSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    inputGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    inputGainAttachment.reset (new SliderAttachment (
                        valueTreeState, "Input Gain", inputGainSlider));

    addAndMakeVisible (inputGainLabel);
    inputGainLabel.setText ("Input Gain", dontSendNotification);
    inputGainLabel.setJustificationType (Justification::centred);
    
    // output gain
    addAndMakeVisible(outputGainSlider);
    outputGainSlider.setSliderStyle(Slider::RotaryVerticalDrag);
    outputGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 80, 20);
    outputGainAttachment.reset (new SliderAttachment (
                        valueTreeState, "Output Gain", outputGainSlider));

    addAndMakeVisible (outputGainLabel);
    outputGainLabel.setText ("Output Gain", dontSendNotification);
    outputGainLabel.setJustificationType (Justification::centred);
 

    // monitoring
    addAndMakeVisible (bufferTimeLabel);
    bufferTimeLabel.setText ("Buffer Time: ", dontSendNotification);
    
    setSize (240, 380);
    startTimerHz (10);
}


TutorialAudioProcessorEditor::~TutorialAudioProcessorEditor()
{
}

//==============================================================================
void TutorialAudioProcessorEditor::paint (Graphics& g)
{
    // input gain
    inputGainLabel.setBounds (30, 30, 80, 30);
    inputGainSlider.setBounds (30, 60, 80, 100);
    
    // output gain
    outputGainLabel.setBounds (130, 30, 80, 30);
    outputGainSlider.setBounds (130, 60, 80, 100);

    // --- monitor --- //
    bufferTimeLabel.setBounds (30, 340, 150, 30);
}

void TutorialAudioProcessorEditor::resized()
{

}

void TutorialAudioProcessorEditor::timerCallback()
{
    float ave = 0.0;
    for(auto i=0; i<100; ++i)
    {
        ave += processor.bufferTimeRecords[i];
    }
    ave /= 100;
    bufferTimeLabel.setText("Buffer Time: " + std::to_string(ave), dontSendNotification);
}
