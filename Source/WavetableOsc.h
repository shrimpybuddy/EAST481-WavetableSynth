#pragma once
#include <JuceHeader.h>
#include <vector>
class WavetableOsc{
public:
	WavetableOsc(std::vector<float> table, double sr, juce::ADSR::Parameters &adsrParams);
	void setFrequency(double freq);
	float getSample();
	void noteOnResetIndex();
	void noteOn();
	void noteOff();
	void stop();
	bool isPlaying();
	bool isActiveButNotPlaying();
	void setAdsrParamsOsc(const juce::ADSR::Parameters& params);
	
private:
	std::vector<float> wavetable;
	double sampleRate;
	double frequency;
	float index = 0.0f;
	float indexInc = 0.0f;
	float linearInterp(float idx);
	juce::ADSR adsr;
	
};

