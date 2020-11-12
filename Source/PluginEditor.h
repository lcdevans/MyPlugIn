/*
  ==============================================================================
    Author: Luke Evans
    Purpose: ECE 484 Final Project Editor Header File (Flanger/Chorus VST3 Plugin)

    The slider definitions are the only changes by the author.

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MyPlugInAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                      private juce::Slider::Listener
{   
public:
    MyPlugInAudioProcessorEditor (MyPlugInAudioProcessor&);
    ~MyPlugInAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MyPlugInAudioProcessor& audioProcessor;

    juce::Slider freqDepth;
    juce::Slider rateCoarse;
    juce::Slider rateFine;
    juce::Slider delayCoarse;
    juce::Slider delayFine;
    juce::Slider phaseBalance;
    juce::Slider delayGain;
    juce::Slider regenGain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyPlugInAudioProcessorEditor)
};
