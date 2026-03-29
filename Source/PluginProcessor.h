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
    { "Devil Chorus",   0.75f, 0.40f, 0.60f, 0.30f, 0.85f },
    { "SAVAGE Hell",    0.90f, 0.85f, 0.70f, 0.80f, 1.00f },
    { "Dark Raw",       0.60f, 0.50f, 0.40f, 0.45f, 0.70f },
    { "Solid Buck",     0.55f, 0.65f, 0.35f, 0.55f, 0.80f },
    { "GNARLY BUCK",    0.95f, 0.90f, 0.80f, 0.90f, 1.00f },
    { "RAZOR EDGE",     0.85f, 0.70f, 0.90f, 0.75f, 0.95f },
    { "POWER VOCAL",    0.50f, 0.80f, 0.20f, 0.85f, 0.75f },
    { "VOID SCREAM",    1.00f, 0.95f, 0.65f, 0.60f, 1.00f },
    { "INDUSTRIAL",     0.80f, 0.75f, 0.85f, 0.50f, 0.90f },
    { "KRUMP KING",     0.88f, 0.82f, 0.55f, 0.88f, 0.92f },
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

    // Gain stages
    juce::dsp::Gain<float> outputGain;

    double currentSampleRate { 44100.0 };
    int    currentBlockSize  { 512 };
    int    currentPreset     { 0 };

    // Dry buffer for wet/dry mix
    juce::AudioBuffer<float> dryBuffer;

    void updateDSP();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WelcomeToRazorAudioProcessor)
};
