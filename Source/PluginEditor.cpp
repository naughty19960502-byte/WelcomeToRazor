#include "PluginEditor.h"

using namespace RazorColours;

// ═════════════════════════════════════════════════════════════════════════════
//  RazorLookAndFeel
// ═════════════════════════════════════════════════════════════════════════════
RazorLookAndFeel::RazorLookAndFeel()
{
    setColour (juce::PopupMenu::backgroundColourId,              juce::Colour (kMetalDark));
    setColour (juce::PopupMenu::textColourId,                    juce::Colour (kLabelText));
    setColour (juce::PopupMenu::highlightedBackgroundColourId,   juce::Colour (kBloodMid));
    setColour (juce::PopupMenu::highlightedTextColourId,         juce::Colours::white);
}

void RazorLookAndFeel::drawGearTeeth (juce::Graphics& g,
                                       float cx, float cy,
                                       float innerR, float outerR,
                                       int numTeeth, juce::Colour col) const
{
    const float twoPi   = juce::MathConstants<float>::twoPi;
    const float step    = twoPi / (float) numTeeth;
    const float toothW  = step * 0.45f;

    juce::Path gear;
    for (int i = 0; i < numTeeth; ++i)
    {
        const float a0 = step * (float)i - toothW * 0.5f;
        const float a1 = a0 + toothW;

        gear.startNewSubPath (cx + std::cos(a0) * innerR, cy + std::sin(a0) * innerR);
        gear.lineTo (cx + std::cos(a0) * outerR, cy + std::sin(a0) * outerR);
        gear.lineTo (cx + std::cos(a1) * outerR, cy + std::sin(a1) * outerR);
        gear.lineTo (cx + std::cos(a1) * innerR, cy + std::sin(a1) * innerR);
        gear.closeSubPath();
    }
    g.setColour (col);
    g.fillPath (gear);
    g.setColour (col.darker (0.5f));
    g.strokePath (gear, juce::PathStrokeType (0.8f));
}

void RazorLookAndFeel::drawBloodSplat (juce::Graphics& g, juce::Random& rng,
                                        float cx, float cy, float radius, int n) const
{
    for (int i = 0; i < n; ++i)
    {
        const float angle = rng.nextFloat() * juce::MathConstants<float>::twoPi;
        const float dist  = rng.nextFloat() * radius;
        const float r     = rng.nextFloat() * 3.5f + 0.8f;
        const float x     = cx + std::cos(angle) * dist;
        const float y     = cy + std::sin(angle) * dist;
        const float alpha = rng.nextFloat() * 0.55f + 0.2f;
        g.setColour (juce::Colour (kBloodMid).withAlpha (alpha));
        g.fillEllipse (x - r, y - r, r * 2.0f, r * 2.0f);
    }
}

void RazorLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                          int x, int y, int width, int height,
                                          float sliderPos,
                                          float startAngle, float endAngle,
                                          juce::Slider&)
{
    const float cx = x + width  * 0.5f;
    const float cy = y + height * 0.5f;
    const float outerR = juce::jmin (width, height) * 0.47f;
    const float gearR  = outerR * 0.87f;
    const float hubR   = gearR  * 0.68f;

    // ── Rust base disk ────────────────────────────────────────────────
    {
        juce::ColourGradient bg (juce::Colour (0xFF2A1808), cx - outerR, cy - outerR,
                                  juce::Colour (0xFF0E0806), cx + outerR, cy + outerR, false);
        g.setGradientFill (bg);
        g.fillEllipse (cx - outerR, cy - outerR, outerR * 2.0f, outerR * 2.0f);
    }

    // ── Outer rust ring ───────────────────────────────────────────────
    g.setColour (juce::Colour (kRust2).withAlpha (0.7f));
    g.drawEllipse (cx - outerR, cy - outerR, outerR * 2.0f, outerR * 2.0f, 3.5f);

    // ── Gear teeth ────────────────────────────────────────────────────
    drawGearTeeth (g, cx, cy, gearR * 0.92f, gearR,
                   24, juce::Colour (kMetalMid));
    drawGearTeeth (g, cx, cy, gearR * 0.92f, gearR,
                   24, juce::Colour (kRust1).withAlpha (0.5f));

    // ── Hub body ──────────────────────────────────────────────────────
    {
        juce::ColourGradient hub (juce::Colour (0xFF302820), cx - hubR * 0.6f, cy - hubR * 0.6f,
                                   juce::Colour (0xFF0A0806), cx + hubR * 0.6f, cy + hubR * 0.6f, false);
        g.setGradientFill (hub);
        g.fillEllipse (cx - hubR, cy - hubR, hubR * 2.0f, hubR * 2.0f);
    }

    // ── Concentric ring detail ─────────────────────────────────────────
    for (float rf : { 0.90f, 0.75f, 0.55f })
    {
        g.setColour (juce::Colour (kMetalMid).withAlpha (0.6f));
        g.drawEllipse (cx - hubR * rf, cy - hubR * rf,
                       hubR * rf * 2.0f, hubR * rf * 2.0f, 1.0f);
    }

    // ── Blood splatter (seeded per-knob by hub position) ──────────────
    {
        juce::Random rng ((juce::int64)(cx * 7919.0f + cy * 31337.0f));
        drawBloodSplat (g, rng, cx, cy, hubR * 0.85f, 8);
    }

    // ── Pointer line ──────────────────────────────────────────────────
    {
        const float angle = startAngle + sliderPos * (endAngle - startAngle)
                            - juce::MathConstants<float>::halfPi;
        const float lineR = hubR * 0.82f;
        const float x1 = cx + std::cos(angle) * hubR * 0.15f;
        const float y1 = cy + std::sin(angle) * hubR * 0.15f;
        const float x2 = cx + std::cos(angle) * lineR;
        const float y2 = cy + std::sin(angle) * lineR;

        // Shadow
        g.setColour (juce::Colours::black.withAlpha (0.7f));
        g.drawLine (x1 + 1.5f, y1 + 1.5f, x2 + 1.5f, y2 + 1.5f, 3.0f);
        // Pointer
        g.setColour (juce::Colour (kLabelText));
        g.drawLine (x1, y1, x2, y2, 2.5f);
        // Tip dot
        g.setColour (juce::Colours::white.withAlpha (0.9f));
        g.fillEllipse (x2 - 2.5f, y2 - 2.5f, 5.0f, 5.0f);
    }

    // ── Centre screw ──────────────────────────────────────────────────
    {
        const float sr = hubR * 0.12f;
        juce::ColourGradient sc (juce::Colour (kRivet), cx - sr, cy - sr,
                                  juce::Colour (0xFF202020), cx + sr, cy + sr, false);
        g.setGradientFill (sc);
        g.fillEllipse (cx - sr, cy - sr, sr * 2.0f, sr * 2.0f);
        g.setColour (juce::Colour (kMetalDark));
        g.drawLine (cx - sr * 0.7f, cy, cx + sr * 0.7f, cy, 1.0f);
    }
}

void RazorLookAndFeel::drawPopupMenuBackground (juce::Graphics& g, int w, int h)
{
    g.setColour (juce::Colour (kMetalDark));
    g.fillAll();
    g.setColour (juce::Colour (kBloodMid));
    g.drawRect (0, 0, w, h, 1);
}

void RazorLookAndFeel::drawPopupMenuItem (juce::Graphics& g,
                                           const juce::Rectangle<int>& area,
                                           bool isSeparator, bool /*isActive*/,
                                           bool isHighlighted, bool isTicked,
                                           bool, const juce::String& text,
                                           const juce::String&, const juce::Drawable*,
                                           const juce::Colour*)
{
    if (isSeparator)
    {
        g.setColour (juce::Colour (kBloodMid).withAlpha (0.4f));
        g.fillRect (area.getX() + 4, area.getCentreY(), area.getWidth() - 8, 1);
        return;
    }
    if (isHighlighted)
    {
        g.setColour (juce::Colour (kBloodMid));
        g.fillRect (area);
    }
    g.setFont (juce::Font (juce::FontOptions (12.0f, juce::Font::bold)));
    g.setColour (isHighlighted ? juce::Colours::white : juce::Colour (kLabelText));
    g.drawText (text, area.withTrimmedLeft (8), juce::Justification::centredLeft);

    if (isTicked)
    {
        g.setColour (juce::Colour (kOrange));
        juce::Path star;
        star.addStar (juce::Point<float> (area.getRight() - 12.0f, (float)area.getCentreY()),
                      3.0f, 6.0f, 5);
        g.fillPath (star);
    }
}

juce::Font RazorLookAndFeel::getPopupMenuFont()
{
    return juce::Font (juce::FontOptions (12.0f, juce::Font::bold));
}

// ═════════════════════════════════════════════════════════════════════════════
//  LCDDisplay
// ═════════════════════════════════════════════════════════════════════════════
LCDDisplay::LCDDisplay() {}

void LCDDisplay::setText (const juce::String& t)
{
    text = t;
    repaint();
}

void LCDDisplay::paint (juce::Graphics& g)
{
    const float w  = (float) getWidth();
    const float h  = (float) getHeight();
    const float bevel = 6.0f;

    // ── Outer metal frame ─────────────────────────────────────────────
    {
        juce::ColourGradient frame (juce::Colour (0xFF504030), 0.0f, 0.0f,
                                     juce::Colour (0xFF201810), w,   h,   false);
        g.setGradientFill (frame);
        g.fillRoundedRectangle (0.0f, 0.0f, w, h, bevel);
    }
    g.setColour (juce::Colour (0xFF604030));
    g.drawRoundedRectangle (0.5f, 0.5f, w - 1.0f, h - 1.0f, bevel, 2.0f);

    // ── Rivets ────────────────────────────────────────────────────────
    auto drawRivet = [&] (float rx, float ry)
    {
        juce::ColourGradient rg (juce::Colour (kRivet), rx - 3, ry - 3,
                                  juce::Colour (0xFF201810), rx + 3, ry + 3, false);
        g.setGradientFill (rg);
        g.fillEllipse (rx - 4, ry - 4, 8.0f, 8.0f);
        g.setColour (juce::Colour (0xFF202020));
        g.drawEllipse (rx - 4, ry - 4, 8.0f, 8.0f, 1.0f);
    };
    drawRivet (10.0f,     10.0f);
    drawRivet (w - 10.0f, 10.0f);
    drawRivet (10.0f,     h - 10.0f);
    drawRivet (w - 10.0f, h - 10.0f);

    // ── LCD glass panel ───────────────────────────────────────────────
    const float px = 14.0f, py = 10.0f;
    const float pw = w - 28.0f, ph = h - 20.0f;
    {
        juce::ColourGradient lcd (juce::Colour (0xFF0D1508), px, py,
                                   juce::Colour (0xFF06100A), px + pw, py + ph, false);
        g.setGradientFill (lcd);
        g.fillRoundedRectangle (px, py, pw, ph, 3.0f);
    }
    // LCD border glow
    g.setColour (juce::Colour (kLCDText).withAlpha (0.25f));
    g.drawRoundedRectangle (px, py, pw, ph, 3.0f, 1.5f);

    // ── Scan lines ────────────────────────────────────────────────────
    g.saveState();
    g.reduceClipRegion ((int)px, (int)py, (int)pw, (int)ph);
    g.setColour (juce::Colours::black.withAlpha (0.18f));
    for (float sy = py; sy < py + ph; sy += 4.0f)
        g.fillRect (px, sy, pw, 2.0f);
    g.restoreState();

    // ── Preset name text ──────────────────────────────────────────────
    juce::Font lcdFont (juce::FontOptions (36.0f, juce::Font::bold));
    g.setFont (lcdFont);

    // Dim ghost layer
    g.setColour (juce::Colour (kLCDDim));
    g.drawText (text, (int)(px + 10), (int)(py + 5), (int)(pw - 20), (int)(ph - 10),
                juce::Justification::centredLeft);

    // Glow halo
    g.setColour (juce::Colour (kLCDText).withAlpha (0.15f));
    for (int blur = 3; blur >= 1; --blur)
        g.drawText (text, (int)(px + 10 - blur), (int)(py + 5),
                    (int)(pw - 20), (int)(ph - 10), juce::Justification::centredLeft);

    // Main text
    g.setColour (juce::Colour (kLCDText));
    g.drawText (text, (int)(px + 10), (int)(py + 5), (int)(pw - 20), (int)(ph - 10),
                juce::Justification::centredLeft);

    // ── Lightning/crack overlay ───────────────────────────────────────
    {
        juce::Random rng (0xC0FFEE42);
        g.setColour (juce::Colour (kLCDText).withAlpha (0.12f));
        float lx = px + pw * 0.7f;
        float ly = py;
        for (int seg = 0; seg < 6; ++seg)
        {
            float nx = lx + rng.nextFloat() * 18.0f - 6.0f;
            float ny = ly + (ph / 6.0f);
            g.drawLine (lx, ly, nx, ny, 0.8f);
            lx = nx; ly = ny;
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  PresetListPanel
// ═════════════════════════════════════════════════════════════════════════════
PresetListPanel::PresetListPanel()
{
    setRepaintsOnMouseActivity (true);
}

void PresetListPanel::setSelectedIndex (int idx)
{
    selectedIdx = idx;
    repaint();
}

void PresetListPanel::paint (juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();

    // Background panel
    juce::ColourGradient bg (juce::Colour (0xFF100C0A), 0.0f, 0.0f,
                              juce::Colour (0xFF0A0806), (float)w, (float)h, false);
    g.setGradientFill (bg);
    g.fillAll();

    g.setColour (juce::Colour (kBloodMid).withAlpha (0.6f));
    g.drawRect (0, 0, w, h, 1);

    // Draw each preset row
    for (int i = 0; i < kNumPresets; ++i)
    {
        const juce::Rectangle<int> row (0, i * kRowH, w, kRowH);

        if (i == selectedIdx)
        {
            juce::ColourGradient rowBg (juce::Colour (kBloodDark).withAlpha (0.8f), 0.0f, (float)row.getY(),
                                         juce::Colour (kBloodMid).withAlpha (0.4f),  (float)w, (float)row.getY(), false);
            g.setGradientFill (rowBg);
            g.fillRect (row);
            g.setColour (juce::Colour (kBloodBright).withAlpha (0.5f));
            g.drawRect (row, 1);
        }
        else if (i == hoveredIdx)
        {
            g.setColour (juce::Colour (kBloodDark).withAlpha (0.4f));
            g.fillRect (row);
        }

        // Separator line
        if (i > 0)
        {
            g.setColour (juce::Colour (kBloodMid).withAlpha (0.2f));
            g.drawHorizontalLine (i * kRowH, 2.0f, (float)(w - 2));
        }

        // Preset name
        const bool isSelected = (i == selectedIdx);
        g.setFont (juce::Font (juce::FontOptions (isSelected ? 13.5f : 13.0f,
                                                  juce::Font::bold)));
        g.setColour (isSelected ? juce::Colour (kPresetHi) : juce::Colour (kPresetText));
        g.drawText (kPresets[i].name,
                    row.withTrimmedLeft (10).withTrimmedRight (22),
                    juce::Justification::centredLeft);

        // Selection marker sigil
        if (isSelected)
        {
            g.setColour (juce::Colour (kOrange));
            juce::Path mark;
            const float mx = (float)(w - 12);
            const float my = row.getCentreY();
            mark.addTriangle (mx - 5, my - 4, mx - 5, my + 4, mx + 4, my);
            g.fillPath (mark);
        }
        else
        {
            // Decorative runic dot
            g.setColour (juce::Colour (kBloodMid).withAlpha (0.5f));
            g.fillEllipse ((float)(w - 12), row.getCentreY() - 2.0f, 4.0f, 4.0f);
        }
    }
}

void PresetListPanel::mouseDown (const juce::MouseEvent& e)
{
    const int idx = e.y / kRowH;
    if (idx >= 0 && idx < kNumPresets)
    {
        selectedIdx = idx;
        repaint();
        if (onSelect) onSelect (selectedIdx);
    }
}

void PresetListPanel::mouseMove (const juce::MouseEvent& e)
{
    const int idx = e.y / kRowH;
    if (idx != hoveredIdx)
    {
        hoveredIdx = (idx >= 0 && idx < kNumPresets) ? idx : -1;
        repaint();
    }
}

void PresetListPanel::mouseExit (const juce::MouseEvent&)
{
    hoveredIdx = -1;
    repaint();
}

// ═════════════════════════════════════════════════════════════════════════════
//  RazorKnob
// ═════════════════════════════════════════════════════════════════════════════
RazorKnob::RazorKnob (const juce::String& labelText) : name (labelText)
{
    slider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider.setRange (0.0, 1.0, 0.001);
    addAndMakeVisible (slider);

    valueLabel.setJustificationType (juce::Justification::centred);
    valueLabel.setColour (juce::Label::textColourId, juce::Colour (kLabelText));
    valueLabel.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    addAndMakeVisible (valueLabel);

    slider.onValueChange = [this]
    {
        valueLabel.setText (juce::String ((int)(slider.getValue() * 100.0)) + "%",
                             juce::dontSendNotification);
    };
    valueLabel.setText ("75%", juce::dontSendNotification);
}

void RazorKnob::resized()
{
    const int h = getHeight();
    const int w = getWidth();
    // Slider takes top portion, value label below, name plate painted
    slider.setBounds (0, 0, w, h - 34);
    valueLabel.setBounds (0, h - 34, w, 16);
}

void RazorKnob::paint (juce::Graphics& g)
{
    const int w  = getWidth();
    const int h  = getHeight();

    // ── Percentage value (already in valueLabel, paint the plate) ─────
    const int plateH = 18;
    const int plateY = h - 18;
    juce::Rectangle<float> plate (2.0f, (float)plateY, w - 4.0f, (float)plateH);

    juce::ColourGradient plateBg (juce::Colour (kLabelBg), 2.0f, (float)plateY,
                                   juce::Colour (0xFF161210), w - 2.0f, (float)(plateY + plateH), false);
    g.setGradientFill (plateBg);
    g.fillRoundedRectangle (plate, 2.0f);
    g.setColour (juce::Colour (kLabelBorder));
    g.drawRoundedRectangle (plate, 2.0f, 1.0f);

    // ── Name label ────────────────────────────────────────────────────
    g.setFont (juce::Font (juce::FontOptions (11.5f, juce::Font::bold)));
    g.setColour (juce::Colour (kLabelText));
    g.drawText (name, 0, plateY + 1, w, plateH - 2, juce::Justification::centred);
}

// ═════════════════════════════════════════════════════════════════════════════
//  WelcomeToRazorAudioProcessorEditor
// ═════════════════════════════════════════════════════════════════════════════
WelcomeToRazorAudioProcessorEditor::WelcomeToRazorAudioProcessorEditor
    (WelcomeToRazorAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&razorLAF);
    setSize (960, 530);

    // OpenGL for hardware-accelerated rendering
    openGLContext.setMultisamplingEnabled (true);
    openGLContext.attachTo (*this);

    // ── Knobs ─────────────────────────────────────────────────────────
    for (auto* k : { &knobGash, &knobBlood, &knobSharpen, &knobRecoil, &knobMixtape })
        addAndMakeVisible (k);

    attGash    = std::make_unique<SliderAttachment> (p.apvts, "GASH",    knobGash.slider);
    attBlood   = std::make_unique<SliderAttachment> (p.apvts, "BLOOD",   knobBlood.slider);
    attSharpen = std::make_unique<SliderAttachment> (p.apvts, "SHARPEN", knobSharpen.slider);
    attRecoil  = std::make_unique<SliderAttachment> (p.apvts, "RECOIL",  knobRecoil.slider);
    attMixtape = std::make_unique<SliderAttachment> (p.apvts, "MIXTAPE", knobMixtape.slider);

    // ── LCD display ───────────────────────────────────────────────────
    addAndMakeVisible (lcdDisplay);
    lcdDisplay.setText (kPresets[p.getCurrentProgram()].name);

    // ── Preset list ───────────────────────────────────────────────────
    addAndMakeVisible (presetPanel);
    presetPanel.setSelectedIndex (p.getCurrentProgram());
    presetPanel.onSelect = [this] (int idx)
    {
        processor.setCurrentProgram (idx);
        lcdDisplay.setText (kPresets[idx].name);
        lastPreset = idx;
    };

    startTimerHz (30);
}

WelcomeToRazorAudioProcessorEditor::~WelcomeToRazorAudioProcessorEditor()
{
    stopTimer();
    openGLContext.detach();
    setLookAndFeel (nullptr);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Timer
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessorEditor::timerCallback()
{
    glowPhase += 0.055f;
    if (glowPhase > juce::MathConstants<float>::twoPi)
        glowPhase -= juce::MathConstants<float>::twoPi;

    // Sync preset list when changed externally (e.g. DAW automation)
    const int cur = processor.getCurrentProgram();
    if (cur != lastPreset)
    {
        lastPreset = cur;
        presetPanel.setSelectedIndex (cur);
        lcdDisplay.setText (kPresets[cur].name);
    }

    repaint (0, 0, 960, 340);   // repaint only animated top area
}

// ─────────────────────────────────────────────────────────────────────────────
//  Layout
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessorEditor::resized()
{
    const int W = getWidth();
    const int H = getHeight();

    // LCD panel — center, below demon face level
    lcdDisplay.setBounds (220, 145, 470, 90);

    // Preset list — right side
    presetPanel.setBounds (W - 215, 145, 208, kNumPresets * 26);

    // 5 knobs — bottom row
    const int knobW    = 110;
    const int knobH    = 120;
    const int knobY    = H - 148;
    const int totalKW  = knobW * 5 + 18 * 4;
    int kx = (W - 215 - totalKW) / 2 + 10;

    for (auto* k : { &knobGash, &knobBlood, &knobSharpen, &knobRecoil, &knobMixtape })
    {
        k->setBounds (kx, knobY, knobW, knobH);
        kx += knobW + 18;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Paint
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessorEditor::paint (juce::Graphics& g)
{
    paintBackground  (g);
    paintChains      (g);
    paintDemonFace   (g);
    paintTitle       (g);
    paintRunicBand   (g);
    paintKrumpText   (g);
    paintCornerRunes (g);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Background
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessorEditor::paintBackground (juce::Graphics& g)
{
    const int W = getWidth();
    const int H = getHeight();

    // ── Base rusted metal plate ───────────────────────────────────────
    juce::ColourGradient bg (juce::Colour (0xFF0F0C09), 0.0f,   0.0f,
                              juce::Colour (0xFF070504), (float)W, (float)H, false);
    g.setGradientFill (bg);
    g.fillAll();

    // ── Rust texture patches ──────────────────────────────────────────
    {
        juce::Random rng (0xFACEFEED);
        for (int i = 0; i < 60; ++i)
        {
            const float rx = rng.nextFloat() * (float)W;
            const float ry = rng.nextFloat() * (float)H;
            const float rr = rng.nextFloat() * 28.0f + 4.0f;
            const float alpha = rng.nextFloat() * 0.12f + 0.02f;
            juce::ColourGradient rust (juce::Colour (kRust2).withAlpha (alpha), rx, ry,
                                        juce::Colours::transparentBlack, rx + rr, ry + rr, true);
            g.setGradientFill (rust);
            g.fillEllipse (rx - rr, ry - rr, rr * 2.0f, rr * 2.0f);
        }
    }

    // ── Blood splatters ───────────────────────────────────────────────
    {
        juce::Random rng (0xDEADBEEF);
        paintBloodSplatter (g, rng,  95.0f, 240.0f, 50.0f, 14);
        paintBloodSplatter (g, rng, 210.0f, 380.0f, 30.0f,  8);
        paintBloodSplatter (g, rng, 580.0f, 195.0f, 25.0f,  6);
        paintBloodSplatter (g, rng, 730.0f, 420.0f, 35.0f, 10);
        paintBloodSplatter (g, rng, 430.0f, 300.0f, 20.0f,  5);
        paintBloodSplatter (g, rng, 840.0f, 270.0f, 18.0f,  6);
    }

    // ── Outer border & rivets ─────────────────────────────────────────
    g.setColour (juce::Colour (0xFF1A1410));
    g.drawRect (0, 0, W, H, 5);
    g.setColour (juce::Colour (0xFF2A2016));
    g.drawRect (5, 5, W - 10, H - 10, 1);

    for (auto [rx, ry] : std::initializer_list<std::pair<int,int>> {
            {16,16}, {W-16,16}, {16,H-16}, {W-16,H-16} })
    {
        juce::ColourGradient cg (juce::Colour (kRivet), (float)rx-4, (float)ry-4,
                                  juce::Colour (0xFF1A1208), (float)rx+4, (float)ry+4, false);
        g.setGradientFill (cg);
        g.fillEllipse ((float)(rx-7), (float)(ry-7), 14.0f, 14.0f);
        g.setColour (juce::Colour (0xFF604030).withAlpha (0.8f));
        g.drawEllipse ((float)(rx-7), (float)(ry-7), 14.0f, 14.0f, 1.5f);
        g.setColour (juce::Colour (0xFF908070).withAlpha (0.5f));
        g.drawLine ((float)rx-3, (float)ry, (float)rx+3, (float)ry, 1.0f);
        g.drawLine ((float)rx, (float)ry-3, (float)rx, (float)ry+3, 1.0f);
    }

    // ── Mid separator ─────────────────────────────────────────────────
    {
        const float sepY = 140.0f;
        juce::ColourGradient sep (juce::Colour (0xFF5A0000), 0.0f, sepY,
                                   juce::Colours::transparentBlack, (float)W * 0.5f, sepY, true);
        g.setGradientFill (sep);
        g.fillRect (0.0f, sepY - 1.0f, (float)W, 2.0f);
    }

    // ── Bottom panel (knob zone) ──────────────────────────────────────
    {
        const int bpY = H - 155;
        juce::ColourGradient bpBg (juce::Colour (0xFF0E0C0A), 0.0f, (float)bpY,
                                    juce::Colour (0xFF080604), 0.0f, (float)H, false);
        g.setGradientFill (bpBg);
        g.fillRect (0, bpY, W - 220, H - bpY);
        g.setColour (juce::Colour (0xFF2A1A08).withAlpha (0.7f));
        g.drawRect (0, bpY, W - 220, H - bpY, 1);
    }
}

void WelcomeToRazorAudioProcessorEditor::paintBloodSplatter (
    juce::Graphics& g, juce::Random& rng,
    float cx, float cy, float maxR, int n)
{
    for (int i = 0; i < n; ++i)
    {
        const float angle = rng.nextFloat() * juce::MathConstants<float>::twoPi;
        const float dist  = rng.nextFloat() * maxR;
        const float r     = rng.nextFloat() * 5.5f + 1.0f;
        const float x     = cx + std::cos(angle) * dist;
        const float y     = cy + std::sin(angle) * dist;
        const float alpha = rng.nextFloat() * 0.5f + 0.08f;
        g.setColour (juce::Colour (kBloodMid).withAlpha (alpha));
        g.fillEllipse (x - r, y - r, r * 2.0f, r * 2.0f);

        if (rng.nextBool())
        {
            const float dripLen = rng.nextFloat() * 22.0f + 5.0f;
            g.setColour (juce::Colour (kBloodDark).withAlpha (alpha * 0.7f));
            g.fillRect (x - 1.0f, y, 2.5f, dripLen);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Demon Face (top-left)
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessorEditor::paintDemonFace (juce::Graphics& g)
{
    const float cx = 112.0f;
    const float cy = 78.0f;

    // ── Shadowy halo behind skull ─────────────────────────────────────
    juce::ColourGradient halo (juce::Colour (kBloodDark).withAlpha (0.5f), cx, cy,
                                juce::Colours::transparentBlack, cx + 90, cy + 90, true);
    g.setGradientFill (halo);
    g.fillEllipse (cx - 90, cy - 90, 180.0f, 200.0f);

    // ── Skull base ────────────────────────────────────────────────────
    {
        juce::ColourGradient skull (juce::Colour (0xFF1E1A14), cx - 55, cy - 40,
                                     juce::Colour (0xFF0A0806), cx + 55, cy + 60, false);
        g.setGradientFill (skull);
        g.fillEllipse (cx - 54, cy - 42, 108.0f, 110.0f);
        g.setColour (juce::Colour (0xFF3A2A18).withAlpha (0.6f));
        g.drawEllipse (cx - 54, cy - 42, 108.0f, 110.0f, 1.5f);
    }

    // ── Horns ─────────────────────────────────────────────────────────
    auto drawHorn = [&] (bool left)
    {
        const float hx  = left ? cx - 36.0f : cx + 36.0f;
        const float hy  = cy - 32.0f;
        const float dir = left ? -1.0f : 1.0f;

        juce::Path horn;
        horn.startNewSubPath (hx, hy);
        horn.cubicTo (hx + dir * 8,  hy - 18,
                      hx + dir * 20, hy - 38,
                      hx + dir * 14, hy - 55);
        horn.lineTo  (hx + dir * 6,  hy - 30);
        horn.cubicTo (hx + dir * 2,  hy - 18,
                      hx - dir * 4,  hy - 8,
                      hx, hy);
        horn.closeSubPath();

        juce::ColourGradient hg (juce::Colour (0xFF2A2010), hx, hy - 55,
                                  juce::Colour (0xFF0E0A06), hx, hy, false);
        g.setGradientFill (hg);
        g.fillPath (horn);
        g.setColour (juce::Colour (0xFF4A3020).withAlpha (0.7f));
        g.strokePath (horn, juce::PathStrokeType (1.2f));
    };
    drawHorn (true);
    drawHorn (false);

    // ── Lower jaw / chin ──────────────────────────────────────────────
    {
        juce::Path jaw;
        jaw.startNewSubPath (cx - 40, cy + 38);
        jaw.quadraticTo (cx, cy + 72, cx + 40, cy + 38);
        jaw.lineTo (cx + 30, cy + 30);
        jaw.quadraticTo (cx, cy + 58, cx - 30, cy + 30);
        jaw.closeSubPath();
        juce::ColourGradient jg (juce::Colour (0xFF181410), cx, cy + 38,
                                  juce::Colour (0xFF080604), cx, cy + 72, false);
        g.setGradientFill (jg);
        g.fillPath (jaw);
    }

    // ── Eyes (glowing) ────────────────────────────────────────────────
    const float eyeGlow = 0.65f + 0.35f * std::sin (glowPhase * 1.3f);
    for (int side : {-1, 1})
    {
        const float ex = cx + side * 20.0f;
        const float ey = cy + 2.0f;

        // Glow corona
        juce::ColourGradient eg (juce::Colour (kGlowEye).withAlpha (eyeGlow * 0.7f),
                                  ex, ey,
                                  juce::Colours::transparentBlack,
                                  ex + 22, ey, true);
        g.setGradientFill (eg);
        g.fillEllipse (ex - 22, ey - 16, 44.0f, 32.0f);

        // Iris
        g.setColour (juce::Colour (0xFFFF5500).withAlpha (eyeGlow));
        g.fillEllipse (ex - 9, ey - 7, 18.0f, 14.0f);

        // Pupil glow
        g.setColour (juce::Colour (0xFFFFAA22).withAlpha (eyeGlow * 0.9f));
        g.fillEllipse (ex - 5, ey - 4, 10.0f, 8.0f);

        // Core bright spot
        g.setColour (juce::Colours::white.withAlpha (eyeGlow * 0.6f));
        g.fillEllipse (ex - 2.5f, ey - 2.0f, 5.0f, 4.0f);
    }

    // ── Nasal cavity ─────────────────────────────────────────────────
    {
        juce::Path nose;
        nose.addTriangle (cx, cy + 18, cx - 6, cy + 28, cx + 6, cy + 28);
        g.setColour (juce::Colours::black.withAlpha (0.8f));
        g.fillPath (nose);
    }

    // ── Teeth / mouth ─────────────────────────────────────────────────
    {
        g.setColour (juce::Colours::black.withAlpha (0.9f));
        g.fillRect (cx - 24.0f, cy + 38.0f, 48.0f, 14.0f);

        g.setColour (juce::Colour (0xFFD0C8B8).withAlpha (0.85f));
        for (int t = 0; t < 7; ++t)
        {
            const float tx = cx - 21.0f + t * 7.0f;
            juce::Path tooth;
            tooth.addTriangle (tx, cy + 38.0f, tx + 6.0f, cy + 38.0f, tx + 3.0f, cy + 50.0f);
            g.fillPath (tooth);
        }
        // Blood between teeth
        g.setColour (juce::Colour (kBloodMid).withAlpha (0.7f));
        for (int t = 0; t < 6; ++t)
        {
            const float tx = cx - 18.0f + t * 7.0f;
            g.fillRect (tx, cy + 44.0f, 1.5f, 6.0f);
        }
    }

    // ── Forehead rune symbol ──────────────────────────────────────────
    {
        g.setColour (juce::Colour (0xFF8B2222).withAlpha (0.75f));
        g.setFont (juce::Font (juce::FontOptions (18.0f, juce::Font::bold)));
        g.drawText (juce::String::fromUTF8 ("✦"), (int)(cx - 12), (int)(cy - 36), 24, 20,
                    juce::Justification::centred);
    }

    // ── Cheekbone / brow ridges ───────────────────────────────────────
    for (int side : {-1, 1})
    {
        juce::Path brow;
        const float bx = cx + side * 28.0f;
        brow.startNewSubPath (bx, cy - 8.0f);
        brow.quadraticTo (bx + side * 14, cy - 18,
                          bx + side * 24, cy - 10);
        g.setColour (juce::Colour (0xFF3A2A18).withAlpha (0.8f));
        g.strokePath (brow, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved,
                                                    juce::PathStrokeType::rounded));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Title
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessorEditor::paintTitle (juce::Graphics& g)
{
    const int W = getWidth();

    // ── Glow pulse ────────────────────────────────────────────────────
    const float glowA = 0.30f + 0.22f * std::sin (glowPhase);
    juce::ColourGradient glow (juce::Colour (0xFFCC0000).withAlpha (glowA),
                                (float)W * 0.42f, 20.0f,
                                juce::Colours::transparentBlack,
                                (float)W * 0.6f, 90.0f, true);
    g.setGradientFill (glow);
    g.fillRect (220, 5, W - 440, 110);

    // ── Title path ────────────────────────────────────────────────────
    juce::Font titleFont (juce::FontOptions (64.0f, juce::Font::bold | juce::Font::italic));

    juce::GlyphArrangement ga;
    ga.addFittedText (titleFont, "WELCOME TO RAZOR",
                      230.0f, 10.0f, (float)(W - 460), 100.0f,
                      juce::Justification::centred, 1);

    juce::Path titlePath;
    ga.createPath (titlePath);

    // Drop shadow
    juce::Path shadow = titlePath;
    juce::AffineTransform shift = juce::AffineTransform::translation (3.0f, 4.0f);
    g.setColour (juce::Colours::black.withAlpha (0.85f));
    g.fillPath (titlePath, shift);

    // Gradient fill (top bright red → dark blood red)
    juce::ColourGradient textGrad (juce::Colour (0xFFFF3A1A), 230.0f, 10.0f,
                                    juce::Colour (0xFF780000), 230.0f, 100.0f, false);
    g.setGradientFill (textGrad);
    g.fillPath (titlePath);

    // Edge highlight
    g.setColour (juce::Colour (0xFFFF9060).withAlpha (0.35f));
    g.strokePath (titlePath, juce::PathStrokeType (0.6f));

    // Blood drips from letters
    {
        juce::Random rng (0xB100D1);
        for (int i = 0; i < 8; ++i)
        {
            const float dx = 250.0f + rng.nextFloat() * (float)(W - 500);
            const float dy = 70.0f + rng.nextFloat() * 20.0f;
            const float dl = rng.nextFloat() * 18.0f + 6.0f;
            const float dw = rng.nextFloat() * 2.5f + 1.0f;
            g.setColour (juce::Colour (kBloodBright).withAlpha (0.8f));
            g.fillRect (dx, dy, dw, dl);
            // Drip bead
            g.fillEllipse (dx - dw, dy + dl, dw * 3.0f, dw * 3.0f);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Chains (diagonal across middle)
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessorEditor::paintChains (juce::Graphics& g)
{
    const int W = getWidth();

    // Chain link parameters
    const float linkW = 20.0f, linkH = 10.0f;
    const float chainY = 255.0f;  // vertical centre of chain

    auto drawChain = [&] (float startX, float endX, float yOffset, float tilt)
    {
        for (float lx = startX; lx < endX; lx += linkW + 2.0f)
        {
            const float ly = chainY + yOffset + std::sin((lx - startX) * tilt) * 6.0f;

            // Link outer
            juce::ColourGradient lg (juce::Colour (kChainHi), lx, ly - linkH,
                                      juce::Colour (kChain),   lx, ly + linkH, false);
            g.setGradientFill (lg);
            g.fillEllipse (lx, ly - linkH * 0.5f, linkW, linkH);
            g.setColour (juce::Colour (kMetalDark));
            g.drawEllipse (lx, ly - linkH * 0.5f, linkW, linkH, 1.5f);

            // Link hole
            g.setColour (juce::Colour (kMetalDark));
            g.fillEllipse (lx + linkW * 0.25f, ly - linkH * 0.25f,
                           linkW * 0.5f, linkH * 0.5f);
        }
    };

    // Two overlapping chains, slightly offset
    drawChain (0.0f,    (float)W * 0.55f, -4.0f,  0.04f);
    drawChain (8.0f,    (float)W * 0.55f,  4.0f,  0.04f);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Runic band (centre horizontal)
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessorEditor::paintRunicBand (juce::Graphics& g)
{
    const int W = getWidth();
    const int H = getHeight();
    const int bandY = H - 165;

    // Band background
    juce::ColourGradient bandBg (juce::Colour (0xFF1A0800).withAlpha (0.5f), 0.0f, (float)bandY,
                                  juce::Colours::transparentBlack, (float)W * 0.5f, (float)bandY, true);
    g.setGradientFill (bandBg);
    g.fillRect (0, bandY, W - 220, 22);

    // Runic text (approximating the image glyphs with similar Unicode)
    g.setFont (juce::Font (juce::FontOptions (14.0f, juce::Font::bold)));
    g.setColour (juce::Colour (0xFF994422).withAlpha (0.8f));
    g.drawText (juce::String::fromUTF8 ("МЕНО  КН\u2716ЕФ  \\\\  ПЮО  RAW"),
                12, bandY + 3, W - 235, 18,
                juce::Justification::centred);
}

// ─────────────────────────────────────────────────────────────────────────────
//  "KRUMP" vertical text (left side)
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessorEditor::paintKrumpText (juce::Graphics& g)
{
    g.saveState();
    g.addTransform (juce::AffineTransform::rotation (
        -juce::MathConstants<float>::halfPi, 50.0f, 295.0f));

    g.setFont (juce::Font (juce::FontOptions (28.0f, juce::Font::bold | juce::Font::italic)));

    // Shadow
    g.setColour (juce::Colours::black.withAlpha (0.7f));
    g.drawText ("KRUMP", 50 - 70 + 2, 295 - 14 + 2, 140, 28, juce::Justification::centred);

    // Glow
    const float ka = 0.55f + 0.25f * std::sin (glowPhase * 0.7f);
    g.setColour (juce::Colour (kBloodBright).withAlpha (ka * 0.4f));
    g.drawText ("KRUMP", 50 - 70 - 1, 295 - 14, 140, 28, juce::Justification::centred);
    g.drawText ("KRUMP", 50 - 70 + 1, 295 - 14, 140, 28, juce::Justification::centred);

    // Main
    g.setColour (juce::Colour (kBloodBright).withAlpha (ka));
    g.drawText ("KRUMP", 50 - 70, 295 - 14, 140, 28, juce::Justification::centred);

    g.restoreState();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Corner rune symbols
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessorEditor::paintCornerRunes (juce::Graphics& g)
{
    const int W = getWidth();

    g.setFont (juce::Font (juce::FontOptions (32.0f, juce::Font::bold)));
    g.setColour (juce::Colour (0xFF6B1A1A).withAlpha (0.65f));

    // Top-right sigil cluster
    g.drawText (juce::String::fromUTF8 ("\u2628\u2716"), W - 85, 12, 72, 36,
                juce::Justification::centred);

    // Left-side runes (ᚱ ᚢ ᚾ ᛖ style — approximated)
    g.setFont (juce::Font (juce::FontOptions (15.0f, juce::Font::bold)));
    g.setColour (juce::Colour (0xFF6B1A1A).withAlpha (0.5f));
    const juce::String leftRunes = juce::String::fromUTF8 ("\u16B1\n\u16AA\n\u16BE\n\u16D6");
    g.drawMultiLineText (leftRunes, 10, 160, 25);

    // Small decorative sigils scattered
    g.setFont (juce::Font (juce::FontOptions (14.0f)));
    g.setColour (juce::Colour (0xFF4A1010).withAlpha (0.55f));
    g.drawText (juce::String::fromUTF8 ("\u2628"), 170, 440, 20, 20, juce::Justification::centred);
    g.drawText (juce::String::fromUTF8 ("\u2720"), 380, 455, 20, 20, juce::Justification::centred);
    g.drawText (juce::String::fromUTF8 ("\u2626"), 610, 445, 20, 20, juce::Justification::centred);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Factory
// ─────────────────────────────────────────────────────────────────────────────
juce::AudioProcessorEditor* WelcomeToRazorAudioProcessor::createEditor()
{
    return new WelcomeToRazorAudioProcessorEditor (*this);
}
