#include "WavetableSynth.h"
WavetableSynth::WavetableSynth(double sr) :
	sampleRate(sr)
{}
void WavetableSynth::handleMidiEvent(juce::MidiMessage event) {
	//getNoteNumber returns an int from 0 - 127, where 60 is middle C
	//each index in our oscillator vector corresponds to a midi note number
	if (event.isNoteOn())
	{
		auto noteNumber = event.getNoteNumber();
		auto frequency = juce::MidiMessage::getMidiNoteInHertz(noteNumber);
		oscillators[noteNumber].setFrequency(frequency);
	}
	else if (event.isAllNotesOff()) {
		for (auto& osc : oscillators) {
			osc.stop();
		}
	}
	else if (event.isNoteOff())
	{
		auto noteNumber = event.getNoteNumber();
		oscillators[noteNumber].stop();
	}

}
void WavetableSynth::setSampleRate(double sr) {
	this->sampleRate = sr;
}

std::vector<float> WavetableSynth::generateSineTable() {
	//fixed wave table lenth of 64
	int length = 64;
	std::vector<float> table(length);

	//for each index, amplitude = sin(2pi(index/length))
	for (int i = 0; i < length; i++) {
		table[i] = std::sin((static_cast<float>(i) / static_cast<float>(length)) * juce::MathConstants<float>::twoPi);
	}
	return table;
}

void WavetableSynth::initOscs() {
	//polyphonic, up to 128 keys pressed, 128 different oscillators
	int numOscs = 128;

	//create wavetable
	auto sineTable = generateSineTable();

	oscillators.clear();
	for (int i = 0; i < numOscs; i++) {
		//emplaceback takes constructor arguments for WavetableOsc and constructs them in place
		oscillators.emplace_back(sineTable, sampleRate);
	}
}

void WavetableSynth::renderAndAppend(juce::AudioBuffer<float>& buffer, int currentSample, int eventSample) {

	//render audio from current sample to event sample
	for (int sample = currentSample; sample < eventSample; sample++) {
		float output = 0.0f;
		for (auto& osc : oscillators) {
			if (osc.isPlaying()) {
				output += osc.getSample();
			}
		}

		//write output to all channels
		for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
			//append output to all channels using write pointer
			buffer.getWritePointer(channel)[sample] += output;
		}
	}
}