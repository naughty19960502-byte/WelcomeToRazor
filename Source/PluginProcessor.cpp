#include "PluginProcessor.h"
#include "PluginEditor.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Parameter layout
// ─────────────────────────────────────────────────────────────────────────────
juce::AudioProcessorValueTreeState::ParameterLayout
WelcomeToRazorAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "GASH",    "Gash",    juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.75f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "BLOOD",   "Blood",   juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.40f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "SHARPEN", "Sharpen", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.60f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "RECOIL",  "Recoil",  juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.30f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "MIXTAPE", "Mixtape", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.85f));

    return { params.begin(), params.end() };
}

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────────────────────────────────────────
WelcomeToRazorAudioProcessor::WelcomeToRazorAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "WelcomeToRazorState", createParameterLayout())
{
    // Saturation: soft clip via tanh
    saturation.functionToUse = [] (float x)
    {
        return std::tanh (x);
    };
}

WelcomeToRazorAudioProcessor::~WelcomeToRazorAudioProcessor() {}

// ─────────────────────────────────────────────────────────────────────────────
//  Prepare
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize  = samplesPerBlock;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels      = 2;

    saturation   .prepare (spec);
    compressor   .prepare (spec);
    filterChain  .prepare (spec);
    presenceChain.prepare (spec);
    outputGain   .prepare (spec);

    outputGain.setGainDecibels (0.0f);

    dryBuffer.setSize (2, samplesPerBlock);

    updateDSP();
}

void WelcomeToRazorAudioProcessor::releaseResources() {}

// ─────────────────────────────────────────────────────────────────────────────
//  updateDSP — maps 0-1 parameter values to DSP settings
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessor::updateDSP()
{
    const float gash    = apvts.getRawParameterValue ("GASH")   ->load();
    const float blood   = apvts.getRawParameterValue ("BLOOD")  ->load();
    const float sharpen = apvts.getRawParameterValue ("SHARPEN")->load();
    const float recoil  = apvts.getRawParameterValue ("RECOIL") ->load();

    // ── GASH: Saturation drive (1x … 20x) ─────────────────────────────
    // The waveshaper itself is tanh; we multiply input by drive before the shaper.
    // We bake the drive into the gain stage that precedes the shaper by
    // storing it in a member that processBlock reads.
    // (done inline in processBlock)

    // ── BLOOD: Compressor ─────────────────────────────────────────────
    // threshold: 0 dB … -40 dB
    // ratio:     2:1  … 40:1
    // attack:    20ms … 1ms
    // release:   200ms … 50ms
    const float threshold = juce::jmap (blood, 0.0f, -40.0f);
    const float ratio     = juce::jmap (blood, 2.0f, 40.0f);
    const float attack    = juce::jmap (blood, 20.0f, 1.0f);
    const float release   = juce::jmap (blood, 200.0f, 50.0f);

    compressor.setThreshold (threshold);
    compressor.setRatio (ratio);
    compressor.setAttack (attack);
    compressor.setRelease (release);

    // ── SHARPEN: High-pass cutoff (80 Hz … 8 kHz) ─────────────────────
    const float hpFreq  = std::exp (juce::jmap (sharpen,
                                                std::log (80.0f),
                                                std::log (8000.0f)));

    *filterChain.get<0>().coefficients =
        *juce::dsp::IIR::Coefficients<float>::makeHighPass (
            currentSampleRate, hpFreq, 0.707f);

    // LP shelf keeps very high end in check (fixed 16 kHz)
    *filterChain.get<1>().coefficients =
        *juce::dsp::IIR::Coefficients<float>::makeLowShelf (
            currentSampleRate, 16000.0f, 0.5f, 0.85f);

    // ── RECOIL: Presence boost at 4 kHz (0 dB … +12 dB) ──────────────
    const float presenceGainDB = juce::jmap (recoil, 0.0f, 12.0f);
    const float presenceGainLinear = juce::Decibels::decibelsToGain (presenceGainDB);

    *presenceChain.get<0>().coefficients =
        *juce::dsp::IIR::Coefficients<float>::makePeakFilter (
            currentSampleRate, 4000.0f, 1.5f, presenceGainLinear);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Bus layout
// ─────────────────────────────────────────────────────────────────────────────
bool WelcomeToRazorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Process block
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                  juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    updateDSP();

    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // ── Store dry signal ──────────────────────────────────────────────
    dryBuffer.setSize (numChannels, numSamples, false, false, true);
    for (int ch = 0; ch < numChannels; ++ch)
        dryBuffer.copyFrom (ch, 0, buffer, ch, 0, numSamples);

    // ── GASH: drive + tanh saturation ─────────────────────────────────
    const float gash  = apvts.getRawParameterValue ("GASH")   ->load();
    const float drive = juce::jmap (gash, 1.0f, 18.0f);   // 1x…18x

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* data = buffer.getWritePointer (ch);
        for (int i = 0; i < numSamples; ++i)
            data[i] = std::tanh (data[i] * drive) / std::tanh (drive);
    }

    // ── BLOOD: compressor ─────────────────────────────────────────────
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        compressor.process (ctx);
    }

    // ── SHARPEN: filter chain ─────────────────────────────────────────
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        filterChain.process (ctx);
    }

    // ── RECOIL: presence EQ ───────────────────────────────────────────
    {
        juce::dsp::AudioBlock<float> block (buffer);
        juce::dsp::ProcessContextReplacing<float> ctx (block);
        presenceChain.process (ctx);
    }

    // ── MIXTAPE: wet/dry blend ────────────────────────────────────────
    const float mix = apvts.getRawParameterValue ("MIXTAPE")->load();
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* wet = buffer.getWritePointer (ch);
        const float* dry = dryBuffer.getReadPointer (ch);
        for (int i = 0; i < numSamples; ++i)
            wet[i] = dry[i] * (1.0f - mix) + wet[i] * mix;
    }

    // ── Output gain: compensate for saturation loudness ───────────────
    buffer.applyGain (juce::Decibels::decibelsToGain (-3.0f));
}

// ─────────────────────────────────────────────────────────────────────────────
//  Presets
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= kNumPresets) return;
    currentPreset = index;

    const auto& p = kPresets[index];
    apvts.getParameterAsValue ("GASH")   .setValue (p.gash);
    apvts.getParameterAsValue ("BLOOD")  .setValue (p.blood);
    apvts.getParameterAsValue ("SHARPEN").setValue (p.sharpen);
    apvts.getParameterAsValue ("RECOIL") .setValue (p.recoil);
    apvts.getParameterAsValue ("MIXTAPE").setValue (p.mixtape);
}

const juce::String WelcomeToRazorAudioProcessor::getProgramName (int index)
{
    if (index >= 0 && index < kNumPresets)
        return kPresets[index].name;
    return {};
}

// ─────────────────────────────────────────────────────────────────────────────
//  State persistence
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty ("currentPreset", currentPreset, nullptr);
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void WelcomeToRazorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
    {
        auto tree = juce::ValueTree::fromXml (*xmlState);
        currentPreset = tree.getProperty ("currentPreset", 0);
        apvts.replaceState (tree);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Factory
// ─────────────────────────────────────────────────────────────────────────────
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WelcomeToRazorAudioProcessor();
}
