/*
  ==============================================================================

    Author: Luke Evans
    Purpose: ECE 484 Final Project Editor (Flanger/Chorus VST3 Plugin)

    The contents of all methods in this file have been written by the author.

    This file contains the basic framework code for a JUCE plugin editor.


  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyPlugInAudioProcessorEditor::MyPlugInAudioProcessorEditor (MyPlugInAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    setSize (600, 400);

    // freqDepth parameters
    freqDepth.setSliderStyle(juce::Slider::Rotary);
    freqDepth.setRotaryParameters(-2.34, 2.34, true);
    freqDepth.setRange(1.0, 1.059, 0.001);
    freqDepth.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 25);
    freqDepth.setPopupDisplayEnabled(true, false, this);
    freqDepth.setValue(1.0);
    addAndMakeVisible(&freqDepth);
    freqDepth.addListener(this);

    // rateCoarse parameters
    rateCoarse.setSliderStyle(juce::Slider::Rotary);
    rateCoarse.setRotaryParameters(-2.34, 2.34, true);
    rateCoarse.setRange(0, 8, 1);
    rateCoarse.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 25);
    rateCoarse.setPopupDisplayEnabled(true, false, this);
    rateCoarse.setTextValueSuffix(" Hz");
    rateCoarse.setValue(1.0);
    addAndMakeVisible(&rateCoarse);
    rateCoarse.addListener(this);

    // rateFine parameters
    rateFine.setSliderStyle(juce::Slider::Rotary);
    rateFine.setRotaryParameters(-2.34, 2.34, true);
    rateFine.setRange(0.1, 1, 0.01);
    rateFine.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 25);
    rateFine.setPopupDisplayEnabled(true, false, this);
    rateFine.setTextValueSuffix(" Hz");
    rateFine.setValue(0.1);
    addAndMakeVisible(&rateFine);
    rateFine.addListener(this);

    // delayCoarse parameters
    delayCoarse.setSliderStyle(juce::Slider::Rotary);
    delayCoarse.setRotaryParameters(-2.34, 2.34, true);
    delayCoarse.setRange(0, 30, 1);
    delayCoarse.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 25);
    delayCoarse.setPopupDisplayEnabled(true, false, this);
    delayCoarse.setTextValueSuffix(" ms");
    delayCoarse.setValue(10);
    addAndMakeVisible(&delayCoarse);
    delayCoarse.addListener(this);

    // delayFine parameters
    delayFine.setSliderStyle(juce::Slider::Rotary);
    delayFine.setRotaryParameters(-2.34, 2.34, true);
    delayFine.setRange(1, 48, 1);
    delayFine.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 25);
    delayFine.setPopupDisplayEnabled(true, false, this);
    delayFine.setTextValueSuffix(" samples");
    delayFine.setValue(1);
    addAndMakeVisible(&delayFine);
    delayFine.addListener(this);

    // phaseBalance parameters
    phaseBalance.setSliderStyle(juce::Slider::Rotary);
    phaseBalance.setRotaryParameters(-2.34, 2.34, true);
    phaseBalance.setRange(0, 180, 1);
    phaseBalance.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 25);
    phaseBalance.setPopupDisplayEnabled(true, false, this);
    phaseBalance.setTextValueSuffix(" degrees");
    phaseBalance.setValue(0.0);
    addAndMakeVisible(&phaseBalance);
    phaseBalance.addListener(this);

    // delayGain parameters
    delayGain.setSliderStyle(juce::Slider::Rotary);
    delayGain.setRotaryParameters(-2.34, 2.34, true);
    delayGain.setRange(0, 1, 0.01);
    delayGain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 25);
    delayGain.setPopupDisplayEnabled(true, false, this);
    delayGain.setTextValueSuffix("");
    delayGain.setValue(0.5);
    addAndMakeVisible(&delayGain);
    delayGain.addListener(this);

    // regenGain parameters
    regenGain.setSliderStyle(juce::Slider::Rotary);
    regenGain.setRotaryParameters(-2.34, 2.34, true);
    regenGain.setRange(0, 0.95, 0.01);
    regenGain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 25);
    regenGain.setPopupDisplayEnabled(true, false, this);
    regenGain.setTextValueSuffix("");
    regenGain.setValue(0.0);
    addAndMakeVisible(&regenGain);
    regenGain.addListener(this);
}

// Pull new values when a slider value changes
void MyPlugInAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.f_ratio = (float)freqDepth.getValue();

    audioProcessor.left_LFO1.resetFrequency( (float)rateCoarse.getValue() + (float)rateFine.getValue() );
    audioProcessor.right_LFO1.resetFrequency( (float)rateCoarse.getValue() + (float)rateFine.getValue() );

    audioProcessor.delayMinimum = 48 * (int)delayCoarse.getValue() + (int)delayFine.getValue();

    audioProcessor.right_LFO1.setPhaseOffset(phaseBalance.getValue() * (float) M_PI / 180.0f);

    audioProcessor.delayGain = delayGain.getValue();

    audioProcessor.regenGain = regenGain.getValue();


};

MyPlugInAudioProcessorEditor::~MyPlugInAudioProcessorEditor()
{
}

//==============================================================================
void MyPlugInAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // Set labels and title
    g.setColour (juce::Colours::white);
    g.setFont(35.0f);
    g.drawFittedText("Luke's Fantastic Flanger", 0, 0, getWidth(), getHeight()-290, juce::Justification::centred, 1);
    g.setFont (20.0f);
    g.drawFittedText("Depth", 35, 130, 60, 30, juce::Justification::centred, 2, 1.0f);
    g.drawFittedText("Rate (Coarse)", 125, 130, 60, 30, juce::Justification::centred, 2, 1.0f);
    g.drawFittedText("Rate (Fine)", 215, 130, 60, 30, juce::Justification::centred, 2, 1.0f);
    g.drawFittedText("Delay (Coarse)", 305, 130, 60, 30, juce::Justification::centred, 2, 1.0f);
    g.drawFittedText("Delay (Fine)", 395, 130, 60, 30, juce::Justification::centred, 2, 1.0f);
    g.drawFittedText("Phase Offset", 485, 130, 60, 30, juce::Justification::centred, 2, 1.0f);
    g.drawFittedText("Delay Gain", 215, 270, 60, 30, juce::Justification::centred, 2, 1.0f);
    g.setFont(15.0f);
    g.drawFittedText("Regeneration", 270, 270, 120, 30, juce::Justification::centred, 2, 1.0f);
}

void MyPlugInAudioProcessorEditor::resized()
{
    // Locations of all rotary sliders
    freqDepth.setBounds(0, 160, getWidth() * .22, getHeight() * .22);
    rateCoarse.setBounds(90, 160, getWidth() * .22, getHeight() * .22);
    rateFine.setBounds(180, 160, getWidth() * .22, getHeight() * .22);
    delayCoarse.setBounds(270, 160, getWidth() * .22, getHeight()* .22);
    delayFine.setBounds(360, 160, getWidth() * .22, getHeight() * .22);
    phaseBalance.setBounds(450, 160, getWidth() * .22, getHeight() * .22);
    delayGain.setBounds(180, 300, getWidth() * .22, getHeight() * .22);
    regenGain.setBounds(270, 300, getWidth() * .22, getHeight() * .22);
}
