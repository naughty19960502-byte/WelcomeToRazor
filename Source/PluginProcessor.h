#pragma once

#include <JuceHeader.h>

// ─────────────────────────────────────────────────────────────────────────────
//  Preset definition
// ─────────────────────────────────────────────────────────────────────────────
struct RazorPreset
{
    const char* name;
    float gash;      // saturation  0-1
    float blood;     // compression 0-1
    float sharpen;   // filter      0-1
    float recoil;    // presence EQ 0-1
    float mixtape;   // wet/dry mix 0-1
};

static constexpr RazorPreset kPresets[] =
{
    //               name           gash  blood sharpen recoil mixtape
    { "Devil Chorus",   0.78f, 0.55f, 0.60f, 0.55f, 0.90f },
    { "SAVAGE Hell",    0.92f, 0.88f, 0.72f, 0.85f, 1.00f },
    { "Dark Raw",       0.70f, 0.62f, 0.42f, 0.60f, 0.82f },
    { "Solid Buck",     0.68f, 0.72f, 0.38f, 0.70f, 0.88f },
    { "GNARLY BUCK",    0.96f, 0.92f, 0.82f, 0.92f, 1.00f },
    { "RAZOR EDGE",     0.88f, 0.75f, 0.92f, 0.80f, 0.96f },
    { "POWER VOCAL",    0.62f, 0.82f, 0.22f, 0.90f, 0.82f },
    { "VOID SCREAM",    1.00f, 0.96f, 0.68f, 0.72f, 1.00f },
    { "INDUSTRIAL",     0.84f, 0.80f, 0.88f, 0.62f, 0.92f },
    { "KRUMP KING",     0.92f, 0.86f, 0.58f, 0.92f, 0.96f },
};

static constexpr int kNumPresets = (int)(sizeof(kPresets) / sizeof(kPresets[0]));

// ─────────────────────────────────────────────────────────────────────────────
//  Processor
// ─────────────────────────────────────────────────────────────────────────────
class WelcomeToRazorAudioProcessor  : public juce::AudioProcessor
{
public:
    WelcomeToRazorAudioProcessor();
    ~WelcomeToRazorAudioProcessor() override;

    //===========================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //===========================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //===========================================================================
    const juce::String getName() const override { return "WelcomeToRazor"; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //===========================================================================
    int getNumPrograms() override    { return kNumPresets; }
    int getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    //===========================================================================
    void getStateInformation  (juce::MemoryBlock& destData) override;
    void setStateInformation  (const void* data, int sizeInBytes) override;

    //===========================================================================
    juce::AudioProcessorValueTreeState apvts;

private:
    //── APVTS layout ────────────────────────────────────────────────────────
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //── DSP chain ───────────────────────────────────────────────────────────
    using Filter    = juce::dsp::IIR::Filter<float>;
    using FilterDup = juce::dsp::ProcessorDuplicator<Filter, juce::dsp::IIR::Coefficients<float>>;

    // Saturation (GASH)
    juce::dsp::WaveShaper<float> saturation;

    // Compression (BLOOD)
    juce::dsp::Compressor<float> compressor;

    // Filter (SHARPEN) – High-pass
    juce::dsp::ProcessorChain<
        juce::dsp::IIR::Filter<float>,   // HP
        juce::dsp::IIR::Filter<float>    // LP shelf
    > filterChain;

    // Presence EQ (RECOIL) – peaking filter per channel
    juce::dsp::ProcessorChain<
        juce::dsp::IIR::Filter<float>
    > presenceChain;

    // Makeup gain (compensates compressor gain reduction)
    juce::dsp::Gain<float> makeupGain;

    // Final output gain
    juce::dsp::Gain<float> outputGain;

    // Output soft clipper / limiter
    juce::dsp::WaveShaper<float> outputClipper;

    double currentSampleRate { 44100.0 };
    int    currentBlockSize  { 512 };
    int    currentPreset     { 0 };

    // Dry buffer for wet/dry mix
    juce::AudioBuffer<float> dryBuffer;

    void updateDSP();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WelcomeToRazorAudioProcessor)
};
