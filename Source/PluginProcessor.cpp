#include "PluginProcessor.h"
#include "PluginEditor.h"

VintageCompressorAudioProcessor::VintageCompressorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       #if ! JucePlugin_IsMidiEffect
                        #if ! JucePlugin_IsSynth
                         .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        #endif
                         .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       #endif
                       ),
#endif
    apvts(*this, nullptr, "Parameters", createParameters())
{
    thresholdParam = apvts.getRawParameterValue("threshold");
    ratioParam     = apvts.getRawParameterValue("ratio");
    attackParam    = apvts.getRawParameterValue("attack");
    releaseParam   = apvts.getRawParameterValue("release");
    kneeParam      = apvts.getRawParameterValue("knee");
    tubeDriveParam = apvts.getRawParameterValue("tubeDrive");
    makeupParam    = apvts.getRawParameterValue("makeup");
}

VintageCompressorAudioProcessor::~VintageCompressorAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout VintageCompressorAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("threshold", "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -24.0f, "dB"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("ratio", "Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.01f), 4.0f, ":1"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f), 10.0f, "ms"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f), 100.0f, "ms"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("knee", "Knee",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 6.0f, "dB"));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("tubeDrive", "Tube Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.2f, ""));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("makeup", "Makeup Gain",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f, "dB"));

    return { params.begin(), params.end() };
}

const juce::String VintageCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VintageCompressorAudioProcessor::acceptsMidi() const
{
   return false;
}

bool VintageCompressorAudioProcessor::producesMidi() const
{
    return false;
}

bool VintageCompressorAudioProcessor::isMidiEffect() const
{
    return false;
}

double VintageCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VintageCompressorAudioProcessor::getNumPrograms()
{
    return 1;
}

int VintageCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VintageCompressorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VintageCompressorAudioProcessor::getProgramName (int index)
{
    return {};
}

void VintageCompressorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void VintageCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lastSampleRate = sampleRate;
    compressor.reset();
    compressor.setAttack(*attackParam);
    compressor.setRelease(*releaseParam);
    compressor.setThreshold(*thresholdParam);
    compressor.setRatio(*ratioParam);
    compressor.setKnee(*kneeParam);
}

void VintageCompressorAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VintageCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VintageCompressorAudioProcessor::updateCompressorSettings()
{
    compressor.setAttack(*attackParam);
    compressor.setRelease(*releaseParam);
    compressor.setThreshold(*thresholdParam);
    compressor.setRatio(*ratioParam);
    compressor.setKnee(*kneeParam);
}

void VintageCompressorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateCompressorSettings();

    // Apply compressor
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float inSample = channelData[sample];

            // Compressor with soft knee
            float threshold = juce::Decibels::decibelsToGain(*thresholdParam);
            float ratio = *ratioParam;
            float knee = *kneeParam;
            float inputDB = juce::Decibels::gainToDecibels(std::abs(inSample) + 1.0e-9f);

            float overThreshold = inputDB - *thresholdParam;
            float gainReduction = 0.0f;

            if (knee > 0.0f)
            {
                if (overThreshold <= -knee/2.0f)
                {
                    gainReduction = 0.0f;
                }
                else if (overThreshold > -knee/2.0f && overThreshold < knee/2.0f)
                {
                    float kneePos = overThreshold + knee/2.0f;
                    gainReduction = ((1.0f/ratio - 1.0f) * kneePos * kneePos) / (2.0f * knee);
                }
                else
                {
                    gainReduction = (1.0f/ratio - 1.0f) * overThreshold;
                }
            }
            else
            {
                if (overThreshold > 0.0f)
                    gainReduction = (1.0f/ratio - 1.0f) * overThreshold;
            }

            float outDB = inputDB + gainReduction;
            float outSample = juce::Decibels::decibelsToGain(outDB) * (inSample < 0.0f ? -1.0f : 1.0f);

            // Tube saturation (vintage tube warmth)
            float drive = *tubeDriveParam * 10.0f + 1.0f;
            float tube = std::tanh(outSample * drive);
            // Subtle even harmonics
            tube = tube + 0.15f * std::pow(tube, 3.0f);

            // Makeup gain
            float makeup = juce::Decibels::decibelsToGain(*makeupParam);
            channelData[sample] = tube * makeup;
        }
    }
}

bool VintageCompressorAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* VintageCompressorAudioProcessor::createEditor()
{
    return new VintageCompressorAudioProcessorEditor (*this);
}

void VintageCompressorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void VintageCompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VintageCompressorAudioProcessor();
}
```

```cpp