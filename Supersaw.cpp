#include "daisy_patch.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

// Controls:
// 1. Frequency
// 2. Detune Coarse (max. 1 octave, i.e. 2x frequency)
// 3. Detune Fine (adds max. 1/10th of Detune Coarse)
// 4. Mix
// Gate In 2: Resets phase of all oscillators (useful for envelope sync)
// Encoder: Switch between supersaw and supersquare
//
// ** Add a dash of reverb ;) **

DaisyPatch patch;

// Variable number of voices in final mix
const unsigned int voices = 7;

// Frequency in Hz
float freq_hz = 150.0f;

// Oscillators
Oscillator osc[voices];

// Waveform
bool saw = true;

// Controls (0.0-1.0)
float ctrl_freq = 0.0f;
float ctrl_detune = 0.0f;
float ctrl_fine = 0.0f;
float ctrl_mix = 0.0f;

void UpdateControls();
void UpdateOled();
float calculateFrequency(unsigned int oscIndex);

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	UpdateControls();

	for (size_t i = 0; i < size; i++)
	{
		float mix = 0;
		
        for(size_t j = 0; j < voices; j++)
        {
			float sig = osc[j].Process();
			float vol = 0.0f;

			if(j == voices / 2) {
				vol = 0.25f * ctrl_mix; // Balanced by ear
			} else {
				vol = (1.0f - ctrl_mix) / (voices - 1);
			}

            mix += sig * vol;
        }

        // Output the mixed oscillators
        out[3][i] = mix;
	}
}

int main(void)
{
	float samplerate;
	patch.Init();
	samplerate = patch.AudioSampleRate();

	patch.SetAudioBlockSize(4); // number of samples handled per callback
	patch.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	for(size_t i = 0; i < voices; i++)
    {
        osc[i].Init(samplerate);
		osc[i].SetWaveform(Oscillator::WAVE_SAW);
        osc[i].SetAmp(.7);
		osc[i].SetFreq(calculateFrequency(i));
    }

	patch.StartAdc();
	patch.StartAudio(AudioCallback);
	
	while(1) {
		UpdateOled();
	}
}

float calculateFrequency(unsigned int oscIndex)
{
	// Range of detuning from lowest to highest oscillator
	float detune_hz = freq_hz * 2; // 1 octave
	float detune_div_hz = detune_hz / (voices - 1);
	float detune_flt = ctrl_detune + (0.1f * ctrl_fine);

	if(oscIndex < voices / 2) {
		return freq_hz - (oscIndex * detune_div_hz * detune_flt);
	} else if(oscIndex > voices / 2) {
		return freq_hz + ((oscIndex - 3) * detune_div_hz * detune_flt);
	}

	return freq_hz;
}

void UpdateOled()
{
    patch.display.Fill(false);

	char cstr[15];
	sprintf(cstr, "Super%s", saw ? "saw" : "square");
	patch.display.SetCursor(0, 0);
	patch.display.WriteString(cstr, Font_6x8, true);

	sprintf(cstr, "FREQ: %d", (int) freq_hz * 10);
	patch.display.SetCursor(0, 8);
	patch.display.WriteString(cstr, Font_6x8, true);

	sprintf(cstr, "DTUN: %d", (int) (ctrl_detune * 100));
	patch.display.SetCursor(0, 16);
	patch.display.WriteString(cstr, Font_6x8, true);

	sprintf(cstr, "FINE: %d", (int) (ctrl_fine * 100));
	patch.display.SetCursor(0, 24);
	patch.display.WriteString(cstr, Font_6x8, true);

	sprintf(cstr, "MIX:  %d", (int) (ctrl_mix * 100));
	patch.display.SetCursor(0, 32);
	patch.display.WriteString(cstr, Font_6x8, true);

    patch.display.Update();
}

void UpdateControls()
{
	patch.ProcessAllControls();

    // Grab control values
	ctrl_freq 	= patch.GetKnobValue((DaisyPatch::Ctrl) 0);
	ctrl_detune = patch.GetKnobValue((DaisyPatch::Ctrl) 1);
	ctrl_fine 	= patch.GetKnobValue((DaisyPatch::Ctrl) 2);
	ctrl_mix 	= patch.GetKnobValue((DaisyPatch::Ctrl) 3);
	
	// Process control values
	freq_hz = (ctrl_freq * 2000);

	// Get gate 2 trigger to reset oscillator phase
	bool trig = patch.gate_input[1].Trig();
	
	// Update oscillators
	for(size_t i = 0; i < voices; i++) {

		// Set oscillator frequency
		osc[i].SetFreq(calculateFrequency(i));

		// Set oscillator waveform
		osc[i].SetWaveform(saw ? 2 : 4);

		// Reset phase
		if(trig) {
			osc[i].Reset();
		}
	}
	
    // Toggle waveform with encoder
    if(patch.encoder.RisingEdge()) {
		saw = !saw;
	}
}
