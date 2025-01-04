#include "daisy_patch.h"
#include "daisysp.h"
#include <algorithm>

using namespace daisy;
using namespace daisysp;

// Controls:
// 1. Frequency
// 2. Detune Coarse (max. 1 octave, i.e. 2x frequency)
// 3. Detune Fine (adds max. 1/10th of Detune Coarse)
// 4. Mix
// Encoder: Switch between supersaw and supersquare
// Gate 1: Triggers amplitude envelope

DaisyPatch patch;

// Variable number of voices in final mix
constexpr unsigned int voices = 7;

// Oscillators
Oscillator osc[voices];

// Envelope
AdEnv env;

// Waveform
bool saw = true;

// Parameters
Parameter freqParam;
Parameter detuneParam;
Parameter fineParam;
Parameter mixParam;

// Parameter values
float freqValue = 0.0f; // Hz
float detuneValue = 0.0f;
float fineValue = 0.0f;
float mixValue = 0.0f;

constexpr float VOLUME = 0.05f;
constexpr float FINE_TUNE_MULTIPLIER = 0.1f;
constexpr float CENTER_OSC_VOLUME = 0.25f;
constexpr size_t CENTER_OSC = voices / 2;

void ProcessControls();
void UpdateOled();
float calculateFrequency(unsigned int oscIndex);
void InitEnvelope(float samplerate);
void InitParameters();

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	ProcessControls();

	for (size_t i = 0; i < size; i++)
	{
		float mix = 0;
		
        for(size_t j = 0; j < voices; j++)
        {
			float sig = osc[j].Process();
			float vol = 0.0f;

			if(j == CENTER_OSC) {
				vol = CENTER_OSC_VOLUME * mixValue; // Balanced by ear
			} else {
				vol = (1.0f - mixValue) / (voices - 1);
			}

            mix += sig * vol;
        }

        // Output the mixed oscillators
        out[3][i] = mix * VOLUME * env.Process();
	}
}

int main(void)
{
	float samplerate;
	patch.Init();
	samplerate = patch.AudioSampleRate();

	patch.SetAudioBlockSize(4); // number of samples handled per callback
	patch.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	for(size_t i = 0; i < voices; i++) {
        osc[i].Init(samplerate);
		osc[i].SetWaveform(Oscillator::WAVE_SAW);
        osc[i].SetAmp(.7);
		osc[i].SetFreq(0.0f);
    }

	InitParameters();
	InitEnvelope(samplerate);

	patch.StartAdc();
	patch.StartAudio(AudioCallback);
	
	while(1) {
		UpdateOled();
	}
}

void InitParameters() {
	freqParam.Init(patch.controls[0], 1.0f, 2000.0f, Parameter::EXPONENTIAL);
	detuneParam.Init(patch.controls[1], 0.0f, 1.0f, Parameter::LINEAR);
	fineParam.Init(patch.controls[2], 0.0f, 1.0f, Parameter::LINEAR);
	mixParam.Init(patch.controls[3], 0.0f, 1.0f, Parameter::LINEAR);
}

void InitEnvelope(float samplerate)
{
    env.Init(samplerate);
	env.SetMax(1);
	env.SetMin(0);
	env.SetCurve(0);

	env.SetTime(ADENV_SEG_ATTACK, 0.01f);
	env.SetTime(ADENV_SEG_DECAY, 0.5f);
}

float calculateFrequency(unsigned int oscIndex, float detune)
{
	if(oscIndex < CENTER_OSC) {
		return freqValue - (oscIndex * detune);
	} else if(oscIndex > CENTER_OSC) {
		return freqValue + ((oscIndex - 3) * detune);
	}

	return freqValue;
}

void DisplayLine(int row, const char* text) {
    patch.display.SetCursor(0, row * 8);
    patch.display.WriteString(text, Font_6x8, true);
}

void DisplayLineText(int row, const char* label) {
    char buffer[20];
    sprintf(buffer, "%s", label);
    DisplayLine(row, buffer);
}

void DisplayLineParameter(int row, const char* label, float value, const char* unit = "") {
	char buffer[20];
    sprintf(buffer, "%s: %d %s", label, static_cast<int>(value), unit);
    DisplayLine(row, buffer);
}

void UpdateOled()
{
    patch.display.Fill(false);

    DisplayLineText(0, saw ? "Supersaw" : "Supersquare");
    DisplayLineParameter(1, "FREQ", freqValue, "Hz");
    DisplayLineParameter(2, "DTUN", detuneValue * 100);
    DisplayLineParameter(3, "FINE", fineValue * 100);
    DisplayLineParameter(4, "MIX", mixValue * 100);

    patch.display.Update();
}

void ProcessControls()
{
	patch.ProcessAllControls();

	// Grab gate 1 trigger
	bool gateTrig = patch.gate_input[0].Trig();
	bool encTrig = patch.encoder.RisingEdge();

	// Grab control values
	freqValue = freqParam.Process();
	detuneValue = detuneParam.Process();
	fineValue = fineParam.Process();
	mixValue = mixParam.Process();

	// Toggle waveform with encoder
    if(encTrig) {
		saw = !saw;
	}

	// Calculate detune amount
	float detune = ((freqValue * 2) / (voices - 1)) * (detuneValue + (FINE_TUNE_MULTIPLIER * fineValue));
	
	// Update oscillators
	for(size_t i = 0; i < voices; i++) {

		// Set oscillator frequency
		osc[i].SetFreq(calculateFrequency(i, detune));

		// Set oscillator waveform
		if(encTrig) {
			osc[i].SetWaveform(saw ? Oscillator::WAVE_SAW : Oscillator::WAVE_SQUARE);
		}

		// Oscillator sync on gate
		if(gateTrig) {
			osc[i].Reset();
		}
	}

	// Trigger envelope on gate 1
	if(gateTrig) {
		env.Trigger();
	}
}
