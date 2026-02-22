#pragma once
#include <JuceHeader.h>
#include <vector>
#include "WavetableOsc.h"

class WavetableSynth
{
public:
	WavetableSynth(double sr);
	void renderAndAppend(juce::AudioBuffer<float>& buffer, int currentSample, int eventSample);
	void handleMidiEvent(juce::MidiMessage event);

	void setSampleRate(double sr);
	void initOscs();
private:
	double sampleRate;
	std::vector<WavetableOsc> oscillators;
	std::vector<float> generateSineTable();
};

