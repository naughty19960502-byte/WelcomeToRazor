#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// ─────────────────────────────────────────────────────────────────────────────
//  Colour palette
// ─────────────────────────────────────────────────────────────────────────────
namespace RazorColours
{
    static constexpr juce::uint32 kBgPlate      = 0xFF0D0B09;
    static constexpr juce::uint32 kRust1        = 0xFF3D1A08;
    static constexpr juce::uint32 kRust2        = 0xFF5C2A0C;
    static constexpr juce::uint32 kMetalDark    = 0xFF1A1614;
    static constexpr juce::uint32 kMetalMid     = 0xFF2E2A26;
    static constexpr juce::uint32 kMetalLight   = 0xFF504840;
    static constexpr juce::uint32 kRivet        = 0xFF706050;
    static constexpr juce::uint32 kBloodDark    = 0xFF5A0000;
    static constexpr juce::uint32 kBloodMid     = 0xFF8B0000;
    static constexpr juce::uint32 kBloodBright  = 0xFFCC2200;
    static constexpr juce::uint32 kOrange       = 0xFFFF6600;
    static constexpr juce::uint32 kGlowEye      = 0xFFFF4400;
    static constexpr juce::uint32 kLCDBg        = 0xFF0A0F06;
    static constexpr juce::uint32 kLCDText      = 0xFF88CC33;
    static constexpr juce::uint32 kLCDDim       = 0xFF2A3A10;
    static constexpr juce::uint32 kPresetText   = 0xFFCC2200;
    static constexpr juce::uint32 kPresetHi     = 0xFFFF4422;
    static constexpr juce::uint32 kLabelBg      = 0xFF100E0C;
    static constexpr juce::uint32 kLabelBorder  = 0xFF3A2010;
    static constexpr juce::uint32 kLabelText    = 0xFFB0A090;
    static constexpr juce::uint32 kChain        = 0xFF282420;
    static constexpr juce::uint32 kChainHi      = 0xFF504840;
}

// ─────────────────────────────────────────────────────────────────────────────
//  LookAndFeel – blood-rusted gear knobs
// ─────────────────────────────────────────────────────────────────────────────
class RazorLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RazorLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h,
                           float sliderPos, float startAngle, float endAngle,
                           juce::Slider&) override;

    void drawPopupMenuBackground (juce::Graphics&, int w, int h) override;

    void drawPopupMenuItem (juce::Graphics&, const juce::Rectangle<int>&,
                            bool isSeparator, bool isActive, bool isHighlighted,
                            bool isTicked, bool hasSubMenu,
                            const juce::String& text, const juce::String&,
                            const juce::Drawable*, const juce::Colour*) override;

    juce::Font getPopupMenuFont() override;

private:
    void drawGearTeeth (juce::Graphics& g, float cx, float cy,
                        float innerR, float outerR, int numTeeth,
                        juce::Colour col) const;
    void drawBloodSplat (juce::Graphics& g, juce::Random& rng,
                         float cx, float cy, float radius, int n) const;
};

// ─────────────────────────────────────────────────────────────────────────────
//  LCD display component (centre panel)
// ─────────────────────────────────────────────────────────────────────────────
class LCDDisplay : public juce::Component
{
public:
    LCDDisplay();
    void setText (const juce::String& t);
    void paint   (juce::Graphics&) override;

private:
    juce::String text { "Devil Chorus" };
};

// ─────────────────────────────────────────────────────────────────────────────
//  Preset list panel (right side, click-to-select)
// ─────────────────────────────────────────────────────────────────────────────
class PresetListPanel : public juce::Component
{
public:
    PresetListPanel();
    void setSelectedIndex (int idx);
    int  getSelectedIndex () const { return selectedIdx; }

    std::function<void(int)> onSelect;

    void paint      (juce::Graphics&) override;
    void mouseDown  (const juce::MouseEvent&) override;
    void mouseMove  (const juce::MouseEvent&) override;
    void mouseExit  (const juce::MouseEvent&) override;

private:
    int selectedIdx { 0 };
    int hoveredIdx  { -1 };
    static constexpr int kRowH = 26;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Knob widget
// ─────────────────────────────────────────────────────────────────────────────
struct RazorKnob : public juce::Component
{
    explicit RazorKnob (const juce::String& labelText);

    juce::Slider slider;
    juce::String name;

    void resized() override;
    void paint   (juce::Graphics&) override;

private:
    juce::Label valueLabel;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Main editor
// ─────────────────────────────────────────────────────────────────────────────
class WelcomeToRazorAudioProcessorEditor : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    explicit WelcomeToRazorAudioProcessorEditor (WelcomeToRazorAudioProcessor&);
    ~WelcomeToRazorAudioProcessorEditor() override;

    void paint   (juce::Graphics&) override;
    void resized () override;

private:
    // ── Timer ──────────────────────────────────────────────────────────
    void timerCallback() override;

    // ── Paint helpers ──────────────────────────────────────────────────
    void paintBackground   (juce::Graphics&);
    void paintDemonFace    (juce::Graphics&);
    void paintTitle        (juce::Graphics&);
    void paintChains       (juce::Graphics&);
    void paintRunicBand    (juce::Graphics&);
    void paintKrumpText    (juce::Graphics&);
    void paintCornerRunes  (juce::Graphics&);
    void paintBloodSplatter(juce::Graphics&, juce::Random&,
                            float cx, float cy, float maxR, int n);

    WelcomeToRazorAudioProcessor& processor;
    RazorLookAndFeel razorLAF;

    // OpenGL context for hardware-accelerated rendering
    juce::OpenGLContext openGLContext;

    // Knobs
    RazorKnob knobGash    { "GASH"    };
    RazorKnob knobBlood   { "BLOOD"   };
    RazorKnob knobSharpen { "SHARPEN" };
    RazorKnob knobRecoil  { "RECOIL"  };
    RazorKnob knobMixtape { "MIXTAPE" };

    // UI panels
    LCDDisplay      lcdDisplay;
    PresetListPanel presetPanel;

    // APVTS attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> attGash, attBlood, attSharpen, attRecoil, attMixtape;

    // Animation
    float glowPhase { 0.0f };
    int   lastPreset { -1  };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WelcomeToRazorAudioProcessorEditor)
};
