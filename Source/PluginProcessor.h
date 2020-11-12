/*
  ==============================================================================
    Author: Luke Evans
    Purpose: ECE 484 Final Project Header File (Flanger/Chorus VST3 Plugin)

    The classes MyDelayLine and LFO are developed entirely by the author.
    The public member variables of the MyPluginAudioProcessor class were added and designed by the author.

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#define _USE_MATH_DEFINES

#include <JuceHeader.h>
#include <iostream>
#include <math.h>
#include <vector>


//==============================================================================
/*
Class: MyDelayLine
       Simple 2-channel delay line
       Circular buffer behaviour
       Fractional delay allowed
*/
class MyDelayLine
{
public:
    // Constructor
    MyDelayLine(int userLength)
    {
        setLength(userLength);
        leftIter = leftLine.begin();
        rightIter = rightLine.begin();
    };

    // Sets the length of the delay line on construction
    void setLength(int userLength)
    {
        length = userLength;
        // Generate vectors of length filled with 0s
        leftLine = std::vector<float>(length, 0);
        rightLine = std::vector<float>(length, 0);
    };

    // Returns the length of the delay line
    int getLength()
    {
        return length;
    };

    // Returns a sample at fractional delay delayChange from channel lineSelect
    // If the delay change is small, just return the zero-delay value
    float getSample(float delayChange, int lineSelect)
    {
        // Split delayChange into integer and fractional components
        float intDelayf, fracDelay;
        fracDelay = modf(delayChange, &intDelayf);
        int intDelay = (int)intDelayf;

        float x1 = 0;

        // Left line
        if (lineSelect == 0) 
        {
            x1 = *leftIter;
            if ( abs(delayChange) < 0.1)
                return x1;
            else 
            {
                // Find the fractional delay
                return getVariableDelay(intDelay, fracDelay, 0);
            }
        }
        // Right line
        else
        {
            x1 = *rightIter;
            if (abs(delayChange) < 0.1)
                return x1;
            else
            {
                // Find the fractional delay
                return getVariableDelay(intDelay, fracDelay, 1);
            }
        }
            
    };

    // Find the fractional delay
    float getVariableDelay(int intDelay, float fracDelay, int channel) {
        
        float returnVal = 0.0f;

        // Left Channel
        if (channel == 0)
        {
            // The delay amount is greater than zero.
            // Should never happen but here as a failsafe.
            if (intDelay >= 0)
            {
                // Positive delay goes past the end of the array
                if (intDelay >= (leftLine.end() - leftIter))
                {
                    returnVal = leftLine.at(intDelay - (leftLine.end() - leftIter)) * (1 - fracDelay) + leftLine.at(intDelay - (leftLine.end() - leftIter)) * fracDelay;
                }
                // Positive delay is at the very end of the array
                else if ((leftIter + intDelay) == leftLine.end() - 1 || (leftIter + intDelay) == leftLine.end() - 2)
                {
                    returnVal = *(leftLine.end() - 1) * (1.0f - fracDelay) + *leftLine.begin() * fracDelay;
                }
                // Positive delay doesn't overlap
                else {
                    returnVal = *(leftIter + intDelay) * (1.0f - fracDelay) + *(leftIter + intDelay + 1) * fracDelay;
                }
            }
            // The delay amount is less than zero
            // This should almost always be the code that actually executes
            else if (intDelay < 0)
            {
                // Negative delay doesnt overlap beginning of array
                if (abs(intDelay) + 1 <= (leftIter - leftLine.begin()))
                {
                    returnVal = *(leftIter - abs(intDelay)) * (1.0f - abs(fracDelay)) + *(leftIter - (abs(intDelay) + 1)) * abs(fracDelay);
                }
                // Negative delay overlaps beginning of array
                else if (abs(intDelay) > (leftIter - leftLine.begin()))
                {
                    returnVal = *(leftLine.end() - 1 - (abs(intDelay) - (leftIter - leftLine.begin()))) * (1.0f - abs(fracDelay)) + *(leftLine.end() - 2 - (abs(intDelay) - (leftIter - leftLine.begin()))) * abs(fracDelay);
                }
                // Negative delay is at very beginning of array
                else {
                    returnVal = *leftLine.begin() * (1.0f - abs(fracDelay)) + *(leftLine.end() - 1) * abs(fracDelay);
                }

            }
        }
        // Right Line
        else
        {
            // The delay amount is greater than zero.
            // Should never happen but here as a failsafe.
            if (intDelay >= 0)
            {
                // Positive delay goes past the end of the array
                if (intDelay >= (rightLine.end() - rightIter))
                {
                    returnVal = rightLine.at(intDelay - (rightLine.end() - rightIter)) * (1 - fracDelay) + rightLine.at(intDelay - (rightLine.end() - rightIter)) * fracDelay;
                }
                // Positive delay is at the very end of the array
                else if ((rightIter + intDelay) == rightLine.end() - 1 || (rightIter + intDelay) == rightLine.end() - 2)
                {
                    returnVal = *(rightLine.end() - 1) * (1.0f - fracDelay) + *rightLine.begin() * fracDelay;
                }
                // Positive delay doesn't overlap
                else {
                    returnVal = *(rightIter + intDelay) * (1.0f - fracDelay) + *(rightIter + intDelay + 1) * fracDelay;
                }
            }
            // The delay amount is less than zero
            // This should almost always be the code that actually executes
            else if (intDelay < 0)
            {
                // Negative delay doesnt overlap beginning of array
                if (abs(intDelay) + 1 <= (rightIter - rightLine.begin()))
                {
                    returnVal = *(rightIter - abs(intDelay)) * (1.0f - abs(fracDelay)) + *(rightIter - (abs(intDelay) + 1)) * abs(fracDelay);
                }
                // Negative delay overlaps beginning of array
                else if (abs(intDelay) > (rightIter - rightLine.begin()))
                {
                    returnVal = *(rightLine.end() - 1 - (abs(intDelay) - (rightIter - rightLine.begin()))) * (1.0f - abs(fracDelay)) + *(rightLine.end() - 2 - (abs(intDelay) - (rightIter - rightLine.begin()))) * abs(fracDelay);
                }
                // Negative delay is at very beginning of array
                else {
                    returnVal = *rightLine.begin() * (1.0f - abs(fracDelay)) + *(rightLine.end() - 1) * abs(fracDelay);
                }

            }

        }

        return returnVal;
    };

    // Set the sample at the current iterator of line lineSelect to newSample
    void setSample(float newSample, int lineSelect)
    {
        // Left Line
        if (lineSelect == 0)
        {
            *leftIter = newSample;
        }
        // Right Line
        else
        {
            *rightIter = newSample;
        }
    };

    // Increment iterator circularly
    void incrementDelay(int lineSelect)
    {
        // Left Line
        if (lineSelect == 0)
        {
            // Loop has occurred, reset to beginning
            if (leftIter >= leftLine.end() - 1)
            {
                leftIter = leftLine.begin();
            }
            // Just iterate
            else
            {
                leftIter++;
            }
        }
        // Right Line
        else
        {
            // Loop has occurred, reset to beginning
            if (rightIter >= rightLine.end() - 1)
            {
                rightIter = rightLine.begin();
            }
            // Just iterate
            else
            {
                rightIter++;
            }
        }
        
    };

    // Return beginning iterator of line lineSelect
    auto getBegin(int lineSelect)
    {
        if (lineSelect == 0) {
            return leftLine.begin();
        }

        return rightLine.begin();
    };

    // Return the current iterator of line lineSelect
    auto getIter(int lineSelect)
    {
        if (lineSelect == 0)
            return leftIter;

        return rightIter;
    };

private:
    // length of delay line
    int length = 0;
    // Iterators for each channel
    std::vector<float>::iterator leftIter;
    std::vector<float>::iterator rightIter;
    // Vectors to act as the circular buffer delay line for each channel
    std::vector<float> leftLine;
    std::vector<float> rightLine;
};

/*
    Low-Frequency Oscillator Object
    Settable frequency and offset
    Cosine waveform
*/
class LFO
{
public: 

    // Constructor
    LFO(float user_f_LFO)
    {
        // Set LFO frequency
        f_LFO = user_f_LFO;
        // Calculate number of steps needed for one period sampled at 48 kHz 
        maxPhaseStep = (int)(f_s / f_LFO);
    }

    // Return the current value of the LFO
    float getCurrentValue()
    {
        return cos( (2.0f * M_PI * (float)phaseStep * ( f_LFO / f_s ) ) + phaseOffset);
    };

    // Set the frequency of the LFO
    // Resets the nominal phase of the LFO if needed
    void resetFrequency(float user_f_LFO)
    {
        // Reset frequency and number of steps needed
        f_LFO = user_f_LFO;
        maxPhaseStep = (int)(f_s / f_LFO);

        // If phase is out of bounds, reset it
        if (phaseStep >= maxPhaseStep - 1)
        {
            phaseStep = 0;
        }
    };

    // Set a new phase offset in radians
    void setPhaseOffset(float user_phaseOffset)
    {
        phaseOffset = user_phaseOffset;
    };

    // Retrieve the phase offset in ratiance
    float getPhaseOffset()
    {
        return phaseOffset;
    };

    // Return the LFO frequency
    float getFrequency() {
        return f_LFO;
    }

    // Circularly increment the LFO
    void incrementLFO()
    {
        // Just increment
        if (phaseStep < maxPhaseStep - 1)
        {
            phaseStep++;
        }
        // Reset if at end
        else if (phaseStep >=  maxPhaseStep - 1)
        {
            phaseStep = 0;
        }
    };

private:
    // Define members needed for operation
    int phaseStep = 0;
    int maxPhaseStep;
    float phaseOffset = 0.0f;
    float f_LFO;
    // Sampling rate fixed to 48 kHz
    float f_s = 48000.0f;
};

class MyPlugInAudioProcessor  : public juce::AudioProcessor
{
public:
    // Define variables needed for access by both the Editor (GUI) and the processor itself
    int delayMinimum = 9;
    float f_ratio = 1.059;
    float left_LFO1_frequency= 1.0f;
    float right_LFO1_frequency = 1.0f;
    float delayGain = 0.8f;
    float regenGain = 0.0f;
    float doubleLFO = 0.0f;
    // Create a 1s long delay line at 48 kHz sampling rate
    MyDelayLine simpleDelay = MyDelayLine(48000);
    // Create left and right LFOs
    LFO left_LFO1 = LFO(left_LFO1_frequency);
    LFO right_LFO1 = LFO(right_LFO1_frequency);
    // Create left and right LFOs for the minimum delay LFOs
    LFO minDelayLFO_left = LFO(0.1f);
    LFO minDelayLFO_right = LFO(0.1f);



    //==============================================================================
    MyPlugInAudioProcessor();
    ~MyPlugInAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyPlugInAudioProcessor)
};

