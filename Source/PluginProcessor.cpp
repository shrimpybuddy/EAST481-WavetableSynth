/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WavetableSynthAudioProcessor::WavetableSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), synth()
    
#endif
{ 
	attackParam = apvts.getRawParameterValue("attack");
	decayParam = apvts.getRawParameterValue("decay");
	sustainParam = apvts.getRawParameterValue("sustain");
	releaseParam = apvts.getRawParameterValue("release");
}

WavetableSynthAudioProcessor::~WavetableSynthAudioProcessor()
{
}

//==============================================================================
const juce::String WavetableSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WavetableSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WavetableSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WavetableSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WavetableSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WavetableSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WavetableSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WavetableSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WavetableSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void WavetableSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WavetableSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    adsrParams = juce::ADSR::Parameters(attackParam->load(), decayParam->load(), sustainParam->load(), releaseParam->load());
	synth.setSampleRate(sampleRate);
	synth.setAdsrParams(adsrParams);
    synth.initOscs();
}

void WavetableSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WavetableSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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

void WavetableSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

	buffer.clear();

    int currentSample = 0;

    //render and repalce audio buffer in between each midi event 
    for(const auto midiMessage : midiMessages)
    {
        const auto event = midiMessage.getMessage();
        const int eventSample = static_cast<int>(midiMessage.samplePosition);//returns number of samples from the start

        if (event.isNoteOn()) {
            //load atomic pointers (which point to apvts parameters) into local adsr params struct for use in synth

            adsrParams.attack = attackParam->load();
            adsrParams.decay = decayParam->load();
            adsrParams.sustain = sustainParam->load();
            adsrParams.release = releaseParam->load();
            //pass local stuct into synth
            synth.setAdsrParams(adsrParams);
        }

        synth.renderAndAppend(buffer, currentSample, eventSample);
        synth.handleMidiEvent(event);

        currentSample = eventSample;
	}
    synth.renderAndAppend(buffer, currentSample, buffer.getNumSamples());
}

//==============================================================================
bool WavetableSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WavetableSynthAudioProcessor::createEditor()
{
    //return new WavetableSynthAudioProcessorEditor (*this);
	return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void WavetableSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream stream(destData, true);
    apvts.state.writeToStream(stream);
}

void WavetableSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ValueTree tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WavetableSynthAudioProcessor();
}
juce::AudioProcessorValueTreeState::ParameterLayout WavetableSynthAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout pl;
    pl.add(
        std::make_unique<juce::AudioParameterFloat>("attack", // parameterID                          
        "Attack", // parameter name
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f),
        0.0f,
        "sec"),
        std::make_unique<juce::AudioParameterFloat>("decay", // parameterID                          
            "Decay", // parameter name
            juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f),
            1.0f,
            "sec"),
        std::make_unique<juce::AudioParameterFloat>("sustain", // parameterID                          
            "Sustain", // parameter name
            juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f),
            0.0f,
            "sec"),
        std::make_unique<juce::AudioParameterFloat>("release", // parameterID                          
            "Release", // parameter name
            juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f),
            1.0f,
            "sec"));
    return pl;  
}
