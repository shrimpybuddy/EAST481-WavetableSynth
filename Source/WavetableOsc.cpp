#include "WavetableOsc.h"
#include <vector>
#include <math.h>

//index inc is freq * length /sampleRate

WavetableOsc::WavetableOsc(std::vector<float> table, double sr)
	: wavetable(std::move(table)), sampleRate(sr), indexInc(0.f)
{
}

void WavetableOsc::setFrequency(double freq)
{
	indexInc = (freq * static_cast<float>(wavetable.size())
		/ static_cast<float>(sampleRate));
}

float WavetableOsc::getSample() {

	const float sample = linearInterp(index);
	
	index += indexInc;
	//index = fmodf(index, static_cast<float>wavetable.size())
	if (index >= wavetable.size()) {
		index -= wavetable.size();
	}
	return sample;
}

float WavetableOsc::linearInterp(float idx) {
	const int leftSampleIdx = static_cast<int>(idx);
	const int rightSampleIdx = static_cast<int>((leftSampleIdx + 1) % wavetable.size());

	const float rightWeight = idx - static_cast<float>(leftSampleIdx);
	const float leftWeight = 1.0f - rightWeight;

	//return (wavetable[leftSampleIdx] * leftWeight) + (wavetable[rightSampleIdx] * rightWeight);
	return wavetable[leftSampleIdx] + rightWeight * (wavetable[rightSampleIdx] - wavetable[leftSampleIdx]);



}
void WavetableOsc::stop() {
	index = 0.f;
	indexInc = 0.f;
}
bool WavetableOsc::isPlaying() {
	return (indexInc != 0.f);
}



