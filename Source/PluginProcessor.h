/*
  ==============================================================================

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
/**
* Class: MyDelayLine
* Simple 2-channel delay line
*/
class MyDelayLine
{
public:
    MyDelayLine(int userLength)
    {
        setLength(userLength);
        leftIter = leftLine.begin();
        rightIter = rightLine.begin();
    };

    void setLength(int userLength)
    {
        length = userLength;
        leftLine = std::vector<float>(userLength, 0);
        rightLine = std::vector<float>(userLength, 0);
    };

    int getLength()
    {
        return length;
    };

    float getSample(float delayChange, int lineSelect)
    {

        float intDelayf, fracDelay;
        fracDelay = modf(delayChange, &intDelayf);
        int intDelay = (int)intDelayf;

        float x1 = 0;

        if (lineSelect == 0) 
        {
            x1 = *leftIter;
            if (delayChange == 0)
                return x1;
            else 
            {
                return getVariableDelay(intDelay, fracDelay, 0);
            }
        }

        else
        {
            x1 = *rightIter;
            if (delayChange == 0)
                return x1;
            else
            {
                return getVariableDelay(intDelay, fracDelay, 1);
            }
        }
            
    };

    float getVariableDelay(int intDelay, float fracDelay, int channel) {
        
        float returnVal = 0;

        if (channel == 0)
        {
            if (intDelay >= (leftLine.end() - leftIter) )
            {
                returnVal = leftLine.at(intDelay - (leftLine.end() - leftIter)) * fracDelay + leftLine.at(intDelay - (leftLine.end() - leftIter)) * (1 - fracDelay);
            }
            else if ((leftIter + intDelay) == leftLine.end() - 1 || (leftIter + intDelay) == leftLine.end() - 2 )
            {
                returnVal = *(leftLine.end() - 1) * fracDelay + *leftLine.begin() * (1 - fracDelay);
            }
            else {
                returnVal = *(leftIter + intDelay) * fracDelay + *(leftIter + intDelay + 1) * (1 - fracDelay);
            }
        }
        else
        {
            if (intDelay >= (rightLine.end() - rightIter))
            {
                returnVal = rightLine.at(intDelay - (rightLine.end() - rightIter)) * fracDelay + rightLine.at(intDelay - (rightLine.end() - rightIter)) * (1 - fracDelay);
            }
            else if ((rightIter + intDelay) == rightLine.end() - 1 || (leftIter + intDelay) == leftLine.end() - 2 )
            {
                returnVal = *(rightLine.end() - 1) * fracDelay + *rightLine.begin() * (1 - fracDelay);
            }
            else {
                returnVal = *(rightIter + intDelay) * fracDelay + *(rightIter + intDelay + 1) * (1 - fracDelay);
            }

        }

        return returnVal;
    };

    void setSample(float newSample, int lineSelect)
    {
        if (lineSelect == 0)
        {
            *leftIter = newSample;
        }
        else
        {
            *rightIter = newSample;
        }
    };


    void incrementDelay(int lineSelect)
    {
        if (lineSelect == 0)
        {
            if (leftIter == leftLine.end() - 1)
            {
                leftIter = leftLine.begin();
            }
            else
            {
                leftIter++;
            }
        }
        else
        {
            if (rightIter == rightLine.end() - 1)
            {
                rightIter = rightLine.begin();
            }
            else
            {
                rightIter++;
            }
        }
        
    };

    auto getBegin(int lineSelect)
    {
        if (lineSelect == 0) {
            return leftLine.begin();
        }

        return rightLine.begin();
    };

    auto getIter(int lineSelect)
    {
        if (lineSelect == 0)
            return leftIter;

        return rightIter;
    };

private:
    int length = 0;
    std::vector<float>::iterator leftIter;
    std::vector<float>::iterator rightIter;
    std::vector<float> leftLine;
    std::vector<float> rightLine;
};

class LFO
{
public: 

    LFO(float user_f_LFO)
    {
        f_LFO = user_f_LFO;
        values = std::vector<float>((int)(f_s / f_LFO), 0);
        populateLFO();
    }

    float getCurrentValue()
    {
        return *LFOIter;
    };

    void resetFrequency(float user_f_LFO)
    {
        f_LFO = user_f_LFO;
        populateLFO();
    };

    float getFrequency() {
        return f_LFO;
    }

    void populateLFO()
    {
        for(int i = 0; i < values.size(); i++)
        {
            values.at(i) = (float)cos(2.0f * M_PI * (float)(i) * (float)(f_LFO / f_s));
        }
        //for (std::vector<float>::iterator iter = values.begin(); iter != values.end(); ++iter)
        //{
            //*iter = (float)cos(2.0f * M_PI * (float)(iter - values.begin()) * f_LFO / f_s);
        //}
        LFOIter = values.begin();
    };

    void incrementLFO()
    {
        if (LFOIter == values.end() - 1) 
        {
            LFOIter = values.begin();
        }
        else
        {
            LFOIter++;
        }
    };

private:
    std::vector<float>::iterator LFOIter;
    std::vector<float> values;
    float f_LFO;
    float f_s = 48000;
};

class MyPlugInAudioProcessor  : public juce::AudioProcessor
{
public:
    float f_ratio = 1.059f;
    float left_LFO1_frequency = 0.5;
    float right_LFO1_frequency = 0.5;
    MyDelayLine simpleDelay = MyDelayLine(3600);
    LFO left_LFO1 = LFO(left_LFO1_frequency);
    LFO right_LFO1 = LFO(right_LFO1_frequency);

    juce::Random randomGenerator;


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

