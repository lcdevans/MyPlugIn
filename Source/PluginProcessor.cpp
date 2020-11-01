/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyPlugInAudioProcessor::MyPlugInAudioProcessor()
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
}

MyPlugInAudioProcessor::~MyPlugInAudioProcessor()
{
}

//==============================================================================
const juce::String MyPlugInAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MyPlugInAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MyPlugInAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MyPlugInAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MyPlugInAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MyPlugInAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MyPlugInAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MyPlugInAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MyPlugInAudioProcessor::getProgramName (int index)
{
    return {};
}

void MyPlugInAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MyPlugInAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void MyPlugInAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MyPlugInAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void MyPlugInAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();

    float delaySample = 0.0f;
    float bufferSample = 0.0f;
    float newValue = 0.0f;
    float delayGain = 0.8f;
    float depthLeftSamples;
    float depthRightSamples;

    depthLeftSamples = 48000.0f * (((float)f_ratio - 1.0f) / (float)(2.0f * M_PI * (float)left_LFO1.getFrequency()));
    depthRightSamples = 48000.0f * (((float)f_ratio - 1.0f) / (float)(2.0f * M_PI * (float)right_LFO1.getFrequency()));

    float delayChange = 0.0f;

    juce::AudioBuffer<float> processBuffer = juce::AudioBuffer<float>(totalNumOutputChannels, numSamples);

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {

        auto* channelData = buffer.getWritePointer (channel);

        for (int index = 0; index < numSamples; index++)
        {
            if (channel == 0) 
            {
                delayChange = ( (float)depthLeftSamples / 2.0f ) * (1.0f + (float)left_LFO1.getCurrentValue());
                left_LFO1.incrementLFO();
            }
            else if (channel == 1) 
            {
                delayChange = ( (float)depthRightSamples / 2.0f ) * (1.0f + (float)right_LFO1.getCurrentValue());
                right_LFO1.incrementLFO();
            }
            delaySample = simpleDelay.getSample(delayChange, channel);
            bufferSample = *buffer.getReadPointer(channel, index);

            newValue = 0.5f*((float)bufferSample + (float)delayGain * (float)delaySample);
            simpleDelay.setSample((bufferSample), channel);
            simpleDelay.incrementDelay(channel);

            channelData[index] = newValue;
        }
     
    }

}

//==============================================================================
bool MyPlugInAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MyPlugInAudioProcessor::createEditor()
{
    return new MyPlugInAudioProcessorEditor (*this);
}

//==============================================================================
void MyPlugInAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MyPlugInAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyPlugInAudioProcessor();
}
