#pragma once
#include <JuceHeader.h>
#include <vector>
#include "WavetableOsc.h"

class WavetableSynth
{
public:
	WavetableSynth();
	WavetableSynth(double sr, juce::ADSR::Parameters &params);
	void renderAndAppend(juce::AudioBuffer<float>& buffer, int currentSample, int eventSample);
	void handleMidiEvent(juce::MidiMessage event);

	void setSampleRate(double sr);
	void initOscs();

	juce::ADSR::Parameters adsrParams;
	void setAdsrParams(juce::ADSR::Parameters& params);
private:
	double sampleRate;
	std::vector<WavetableOsc> oscillators;
	std::vector<float> generateSineTable();
	juce::SmoothedValue<float> normalizationSmooth;
};

