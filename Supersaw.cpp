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
constexpr unsigned int voices = 7;
constexpr unsigned int voices_half = voices / 2;

// Oscillators
Oscillator osc[voices];

// Envelope
AdEnv env;

// State-variable filters
Svf svfLeft;
Svf svfRight;
AdEnv filterEnv;

// Waveform
bool saw = true;

// Frequency
float freq = 0.0f; // Hz

// Parameter values
float freqValue = 0.0f;
float detuneValue = 0.0f;
float filterFreq = 0.0f;
float widthValue = 1.0f;

constexpr float VOLUME = 0.03f; // Use 0.03f or lower for line level
constexpr float FREQ_MIN = 30.0f;
constexpr float FREQ_MAX = 2000.0f;
constexpr float POT_OFFSET = 0.01f; // Min-max pinning to reach 0.0 and 1.0
constexpr float DETUNE_RANGE = 2.5f;
constexpr float FLT_FREQ_MAX = 15000.0f;

constexpr int NUM_POTS = 4; // Sources

// Unison parameters
float panL[voices] = {1.0f};
float panR[voices] = {1.0f};
float detune_bias = 1.0f;
float detune_offset = 0.0f;

enum ParameterIndex {
    FREQ = 0,
    DETUNE,
    AMP_ENV,
    FLT_FREQ,
    WIDTH,
    COUNT // Destinations
};

// Parameter values
std::array<float, ParameterIndex::COUNT> parameters = {0.0f};

// Modulation matrix: maps potentiometer indices to value indices
std::array<int, NUM_POTS> modMatrix = {
    ParameterIndex::FREQ,
    ParameterIndex::DETUNE,
    ParameterIndex::FLT_FREQ,
    ParameterIndex::WIDTH,
};

void ProcessControls();
void UpdateOled();
void InitAmpEnvelope(float samplerate);
void InitFilter(float samplerate);
void InitFilterEnvelope(float samplerate);
void InitUnison(int voices, float width);

float mapValue(float value, float outMin, float outMax) {
    return fminf(fmaxf((value - POT_OFFSET) / ((outMax - POT_OFFSET) - POT_OFFSET) * (outMax - outMin) + outMin, outMin), outMax);
}

float mapValueExponential(float value, float min, float max) {
    return mapValue(((value * value) * (max - min)) + min, min, max);
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    ProcessControls();

    // Update unison with dynamic width
    InitUnison(voices, parameters[ParameterIndex::WIDTH]);

    // Use CV envelope or Gate 1
    float ampEnv = parameters[ParameterIndex::AMP_ENV] > 0 ? parameters[ParameterIndex::AMP_ENV] : env.Process();
    float filterEnvValue = filterEnv.Process();

    // Write to CV1 and CV2
    patch.seed.dac.WriteValue(DacHandle::Channel::ONE, ampEnv * 4095);
    patch.seed.dac.WriteValue(DacHandle::Channel::TWO, filterEnvValue * 4095);

    // Modulate filter frequency
    float modulatedFrequency = (filterFreq * FLT_FREQ_MAX) * (1.0f + filterEnvValue);
    svfLeft.SetFreq(modulatedFrequency);
    svfRight.SetFreq(modulatedFrequency);

    for (size_t i = 0; i < size; i++)
    {
        float left = 0.0f;
        float right = 0.0f;

        for (size_t j = 0; j < voices; j++)
        {
            float sig = osc[j].Process();

            // Apply panning and normalize volumes
            left += sig * panL[j] * (1.0f / voices);
            right += sig * panR[j] * (1.0f / voices);
        }

        // Process filters independently
        svfLeft.Process(left);
        svfRight.Process(right);

        // Apply volume and amplitude envelope
        left = svfLeft.Low() * ampEnv * VOLUME;
        right = svfRight.Low() * ampEnv * VOLUME;

        // Output to left and right channels
        out[0][i] = left;
        out[1][i] = right;
        out[2][i] = left;
        out[3][i] = right;
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

    InitAmpEnvelope(samplerate);
    InitFilter(samplerate);
    InitFilterEnvelope(samplerate);

    patch.StartAdc();
    patch.StartAudio(AudioCallback);
    
    while(1) {
        UpdateOled();
    }
}

void InitAmpEnvelope(float samplerate)
{
    env.Init(samplerate);
    env.SetMax(1);
    env.SetMin(0);
    env.SetCurve(0.0f); // snappy: -50.0f

    env.SetTime(ADENV_SEG_ATTACK, 0.01f);
    env.SetTime(ADENV_SEG_DECAY, 1.0f);
}

void InitFilter(float samplerate) 
{
    svfLeft.Init(samplerate);
    svfLeft.SetRes(0.0f);
    svfLeft.SetDrive(0.0f);

    svfRight.Init(samplerate);
    svfRight.SetRes(0.0f);
    svfRight.SetDrive(0.0f);
}

// Initialize the filter envelope
void InitFilterEnvelope(float samplerate)
{
    filterEnv.Init(samplerate);
    filterEnv.SetMax(100.0f);  // Maximum modulation value
    filterEnv.SetMin(0.0f);  // Minimum modulation value
    filterEnv.SetCurve(-50.0f); // Linear envelope
    filterEnv.SetTime(ADENV_SEG_ATTACK, 0.01f); // Quick attack
    filterEnv.SetTime(ADENV_SEG_DECAY, 0.5f);   // Short decay
}

void InitUnison(int voices, float width)
{
    detune_bias = 2.0f / (voices - 1.0f);
    detune_offset = -1.0f;

    float mid = (voices - 1) * 0.5f;

    for (int i = 0; i < voices; ++i)
    {
        float d = (i - mid) / mid; // Normalized position from -1.0 to 1.0

        // Alternate panning based on index parity
        if (i % 2 == 0) // Even-indexed oscillators
        {
            panL[i] = 1.0f - (d * width); // Slightly biased to left
            panR[i] = 0.8f + (d * width); // Less biased to right
        }
        else // Odd-indexed oscillators
        {
            panL[i] = 0.8f - (d * width); // Less biased to left
            panR[i] = 1.0f + (d * width); // Slightly biased to right
        }

        // Clamp pan values between 0.0 and 1.0 for safety
        panL[i] = std::max(0.0f, std::min(1.0f, panL[i]));
        panR[i] = std::max(0.0f, std::min(1.0f, panR[i]));
    }
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

    patch.display.Update();
}

// Update modulation matrix (assign pots to different values dynamically)
void SetPotMapping(int potIndex, int valueIndex) {
    if (potIndex < NUM_POTS && valueIndex < ParameterIndex::COUNT) {
        modMatrix[potIndex] = valueIndex;
    }
}

// float calculateFrequency(unsigned int oscIndex)
// {
//     // Detuning logic for oscillators below and above the center
//     if (oscIndex < voices_half) {
//         return freq - (DETUNE_RANGE * (voices_half - oscIndex) * detuneValue);
//     } else {
//         return freq + (DETUNE_RANGE * (oscIndex - voices_half) * detuneValue);
//     }

//     // Center oscillator (no detuning)
//     return freq;
// }

float calculateFrequency(unsigned int oscIndex) {
    float detuneAmount = detune_offset + (detune_bias * oscIndex);
    float randomOffset = ((rand() % 100) / 100.0f) - 0.5f; // Random range: [-0.5, 0.5]
    return freq + ((detuneAmount + randomOffset) * DETUNE_RANGE * detuneValue);
}

void ProcessControls()
{
    patch.ProcessAllControls();

    for (int potIndex = 0; potIndex < NUM_POTS; ++potIndex) {

        // Read potentiometer value
        float potValue = patch.GetKnobValue(static_cast<DaisyPatch::Ctrl>(potIndex));

        // Map and clamp value (add exponential mapping where necessary)
        potValue = mapValue(potValue, 0.0f, 1.0f);

        // Find the corresponding parameter index
        int valueIndex = modMatrix[potIndex];

        // Update the parameter value
        parameters[valueIndex] = potValue;
    }

    // Grab gate 1 and 2 triggers
    bool gateTrig1 = patch.gate_input[0].Trig();
    bool gateTrig2 = patch.gate_input[1].Trig();

    // Grab encoder trigger
    bool encTrig = patch.encoder.RisingEdge();

    // Grab parameter values
    freqValue = mapValueExponential(parameters[ParameterIndex::FREQ], 0.0f, 1.0f);
    detuneValue = mapValueExponential(parameters[ParameterIndex::DETUNE], 0.0f, 1.0f);
    widthValue = parameters[ParameterIndex::WIDTH];

    // Filter
    filterFreq = mapValueExponential(parameters[ParameterIndex::FLT_FREQ], 0.0f, 1.0f);
    
    // Toggle waveform with encoder
    if(encTrig) {
        saw = !saw;
    }

    // Calculate frequency in Hz
    freq = FREQ_MIN + (freqValue * FREQ_MAX);

    // Update oscillators
    for(size_t i = 0; i < voices; i++) {

        // Set oscillator frequency
        osc[i].SetFreq(calculateFrequency(i));

        // Set oscillator waveform
        if(encTrig) {
            osc[i].SetWaveform(saw ? Oscillator::WAVE_SAW : Oscillator::WAVE_SQUARE);
        }

        // Oscillator sync on gate
        if(gateTrig1 || gateTrig2) {
            osc[i].Reset();
        }
    }

    // Trigger envelope on gate 1
    if(gateTrig1) {
        env.Trigger();
        filterEnv.Trigger();
    }
}