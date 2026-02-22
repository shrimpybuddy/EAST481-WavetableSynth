#pragma once
#include <JuceHeader.h>
#include <vector>
class WavetableOsc{
public:
	WavetableOsc(std::vector<float> table, double sr);
	void setFrequency(double freq);
	float getSample();
	void stop();
	bool isPlaying();
	
private:
	std::vector<float> wavetable;
	double sampleRate;
	double frequency;
	float index = 0.0f;
	float indexInc = 0.0f;
	float linearInterp(float idx);
	
};

