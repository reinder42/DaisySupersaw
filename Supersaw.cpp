#include "daisy_patch.h"
#include "daisysp.h"
#include <algorithm>

#include "util/custom_font.c"

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
constexpr unsigned int voices = 15;

// Oscillators
Oscillator osc[voices];

// Envelope
AdEnv env;

// Waveform
bool saw = true;

// Frequency
float freq = 0.0f; // Hz

// Parameter values
float freqValue = 0.0f;
float detuneValue = 0.0f;
float fineValue = 0.0f;
float mixValue = 0.0f;
float volumeValue = 0.0f;
float ampEnvValue = 1.0f;

constexpr float DEFAULT_VOLUME = 0.04f;
constexpr float CENTER_OSC_VOLUME = 0.25f;
constexpr float FREQ_MIN = 30.0f;
constexpr float FREQ_MAX = 2000.0f;
constexpr float POT_OFFSET = 0.02f; // Min-max pinning to reach 0.0 and 1.0
constexpr float DETUNE_RANGE = 10.0f;

void ProcessControls();
void UpdateOled();
void InitEnvelope(float samplerate);

float mapValue(float value, float outMin, float outMax) {
    return fminf(fmaxf((value - POT_OFFSET) / ((outMax - POT_OFFSET) - POT_OFFSET) * (outMax - outMin) + outMin, outMin), outMax);
}

float mapValueExponential(float value, float min, float max) {
    return mapValue(((value * value) * (max - min)) + min, min, max);
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    ProcessControls();
    
    // Envelope from gate 1
    float gateEnv = env.Process();

    for (size_t i = 0; i < size; i++)
    {
        float mix = 0;
        
        for(size_t j = 0; j < voices; j++)
        {
            float sig = osc[j].Process();
            float vol = j == 0 ? CENTER_OSC_VOLUME * mixValue : (1.0f - mixValue) / (voices - 1);

            mix += sig * vol;
        }

        // Output the mixed oscillators
        out[3][i] = mix * volumeValue * gateEnv * ampEnvValue;
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

    volumeValue = DEFAULT_VOLUME;

    InitEnvelope(samplerate);

    patch.StartAdc();
    patch.StartAudio(AudioCallback);
    
    while(1) {
        UpdateOled();
    }
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

void DisplayLine(int row, const char* text) {
    patch.display.SetCursor(0, row * 8);
    patch.display.WriteString(text, CustomFont_6x8, true);
}

void DisplayLineText(int row, const char* label) {
    char buffer[20];
    sprintf(buffer, "%s", label);
    DisplayLine(row, buffer);
}

void DisplayLineParameter(int row, const char* label, float value, const char* unit = "") {
    char buffer[20];
    sprintf(buffer, "%s:%d %s", label, static_cast<int>(value), unit);
    DisplayLine(row, buffer);
}

void UpdateOled()
{
    patch.display.Fill(false);

    DisplayLineText(0, saw ? "Supersaw" : "Supersquare");
    DisplayLineParameter(1, "FREQ", freq, "Hz");
    DisplayLineParameter(2, "DTUN", detuneValue * 100);
    DisplayLineParameter(3, "FINE", fineValue * 100);
    DisplayLineParameter(4, "MIX", mixValue * 100);
    DisplayLineParameter(5, "VOL", volumeValue * 100);

    patch.display.Update();
}

void ProcessControls()
{
    patch.ProcessAllControls();

    // Grab gate 1 trigger
    bool gateTrig = patch.gate_input[0].Trig();
    bool encTrig = patch.encoder.RisingEdge();

    // Grab control values
    freqValue 	= patch.GetKnobValue((DaisyPatch::Ctrl) 0);
	detuneValue = patch.GetKnobValue((DaisyPatch::Ctrl) 1);
	fineValue 	= patch.GetKnobValue((DaisyPatch::Ctrl) 2);
	mixValue 	= patch.GetKnobValue((DaisyPatch::Ctrl) 3);

    // Pin (by POT_OFFSET), map and clamp between .0f and 1.0f
    freqValue = mapValueExponential(freqValue, 0.0f, 1.0f);
    detuneValue = mapValue(detuneValue, 0.0f, 1.0f);
    fineValue = mapValue(fineValue, 0.0f, 1.0f);
    mixValue = mapValue(mixValue, 0.0f, 1.0f);

    // Toggle waveform with encoder
    if(encTrig) {
        saw = !saw;
    }

    // Calculate frequency in Hz
    freq = FREQ_MIN + (freqValue * FREQ_MAX);

    // Range of detuning from lowest to highest oscillator
	float detune = 10.0f / (voices - 1);
    unsigned int voices_half = voices / 2;

    // Update oscillators
    for(size_t i = 0; i < voices; i++) {

        // Set oscillator frequency
        float voice_freq = freq;

        if(i < voices_half) {
            voice_freq = freq - (i * detune * detuneValue);
        } else if(i > voices_half) {
            voice_freq = freq + ((i - (voices_half)) * detune * detuneValue);
        } 

        osc[i].SetFreq(voice_freq);

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