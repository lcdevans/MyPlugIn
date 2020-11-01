/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyPlugInAudioProcessorEditor::MyPlugInAudioProcessorEditor (MyPlugInAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    // freqDepth parameters
    freqDepth.setSliderStyle(juce::Slider::LinearBarVertical);
    freqDepth.setRange(1.0, 1.059, 0.001);
    freqDepth.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    freqDepth.setPopupDisplayEnabled(true, false, this);
    freqDepth.setValue(1.05);

    // LFOFreq parameters
    LFOFreq.setSliderStyle(juce::Slider::LinearBarVertical);
    LFOFreq.setRange(0.25, 7, 0.01);
    LFOFreq.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    LFOFreq.setPopupDisplayEnabled(true, false, this);
    LFOFreq.setTextValueSuffix("Hz");
    LFOFreq.setValue(1.0);

    // Make Visible
    addAndMakeVisible(&freqDepth);
    addAndMakeVisible(&LFOFreq);

    freqDepth.addListener(this);
    LFOFreq.addListener(this);
}

void MyPlugInAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.f_ratio = (float)freqDepth.getValue();
    //if (LFOFreq.getValue() >= ((float)audioProcessor.f_ratio - 1.0f)/(M_PI * (float)audioProcessor.delayLength) * 48000.0f) {
        audioProcessor.left_LFO1.resetFrequency((float)LFOFreq.getValue());
        audioProcessor.right_LFO1.resetFrequency((float)LFOFreq.getValue());
    //}

};

MyPlugInAudioProcessorEditor::~MyPlugInAudioProcessorEditor()
{
}

//==============================================================================
void MyPlugInAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Depth", 40, 30, getWidth(), 30, juce::Justification::centred, 1 );
    g.drawFittedText("Rate", 80, 30, getWidth(), 30, juce::Justification::centred, 1);
}

void MyPlugInAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    freqDepth.setBounds(40, 30, 20, getHeight() - 60);
    LFOFreq.setBounds(80, 30, 20, getHeight() - 60);
}
