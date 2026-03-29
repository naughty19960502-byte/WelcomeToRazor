#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Custom LookAndFeel – Industrial Razor style
// ─────────────────────────────────────────────────────────────────────────────
class RazorLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RazorLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider&) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox&) override;

    void drawPopupMenuBackground (juce::Graphics&, int width, int height) override;

    void drawPopupMenuItem (juce::Graphics&, const juce::Rectangle<int>&,
                            bool isSeparator, bool isActive, bool isHighlighted,
                            bool isTicked, bool hasSubMenu,
                            const juce::String& text, const juce::String& shortcutKeyText,
                            const juce::Drawable* icon,
                            const juce::Colour* textColour) override;

    juce::Font getComboBoxFont (juce::ComboBox&) override;
    juce::Font getPopupMenuFont() override;

private:
    // Palette
    static constexpr juce::uint32 kBgColour      = 0xFF0A0A0A;
    static constexpr juce::uint32 kPanelColour    = 0xFF141414;
    static constexpr juce::uint32 kMetalDark      = 0xFF1E1E1E;
    static constexpr juce::uint32 kMetalMid       = 0xFF2E2E2E;
    static constexpr juce::uint32 kMetalLight     = 0xFF454545;
    static constexpr juce::uint32 kSilver         = 0xFFC0C0C0;
    static constexpr juce::uint32 kOrange         = 0xFFFF6600;
    static constexpr juce::uint32 kBloodRed       = 0xFF8B0000;
    static constexpr juce::uint32 kBrightRed      = 0xFFCC0000;
    static constexpr juce::uint32 kGlowRed        = 0xFFFF2020;
    static constexpr juce::uint32 kTextColour     = 0xFFD0D0D0;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Knob with label
// ─────────────────────────────────────────────────────────────────────────────
struct RazorKnob : public juce::Component
{
    explicit RazorKnob (const juce::String& labelText);

    juce::Slider slider;
    juce::Label  valueLabel;
    juce::String name;

    void resized() override;
    void paint  (juce::Graphics&) override;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Main Editor
// ─────────────────────────────────────────────────────────────────────────────
class WelcomeToRazorAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                            private juce::Timer
{
public:
    explicit WelcomeToRazorAudioProcessorEditor (WelcomeToRazorAudioProcessor&);
    ~WelcomeToRazorAudioProcessorEditor() override;

    void paint   (juce::Graphics&) override;
    void resized () override;

private:
    void timerCallback() override;
    void buildPresetBox();

    // ── Background paint helpers ─────────────────────────────────────────
    void paintBackground (juce::Graphics&);
    void paintTitle      (juce::Graphics&);
    void paintBloodSplatter (juce::Graphics& g, juce::Random& rng,
                              float cx, float cy, float maxRadius, int numDrops);
    void paintRunicText  (juce::Graphics&);
    void paintPresetsPanel (juce::Graphics&);
    void paintDemonMask  (juce::Graphics&);

    WelcomeToRazorAudioProcessor& processor;
    RazorLookAndFeel razorLAF;

    // 5 knobs
    RazorKnob knobGash    { "GASH"    };
    RazorKnob knobBlood   { "BLOOD"   };
    RazorKnob knobSharpen { "SHARPEN" };
    RazorKnob knobRecoil  { "RECOIL"  };
    RazorKnob knobMixtape { "MIXTAPE" };

    juce::ComboBox presetBox;

    // APVTS attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> attGash, attBlood, attSharpen, attRecoil, attMixtape;

    // Animation
    float glowPhase { 0.0f };

    // Cached background image for performance
    juce::Image bgCache;
    bool bgDirty { true };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WelcomeToRazorAudioProcessorEditor)
};
