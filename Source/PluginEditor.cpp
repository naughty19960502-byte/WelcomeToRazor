#include "PluginEditor.h"

// ═════════════════════════════════════════════════════════════════════════════
//  RazorLookAndFeel
// ═════════════════════════════════════════════════════════════════════════════
RazorLookAndFeel::RazorLookAndFeel()
{
    setColour (juce::Slider::thumbColourId,              juce::Colour (kSilver));
    setColour (juce::Slider::rotarySliderFillColourId,   juce::Colour (kOrange));
    setColour (juce::Slider::rotarySliderOutlineColourId,juce::Colour (kMetalMid));
    setColour (juce::ComboBox::backgroundColourId,        juce::Colour (kMetalDark));
    setColour (juce::ComboBox::outlineColourId,           juce::Colour (kBloodRed));
    setColour (juce::ComboBox::textColourId,              juce::Colour (kSilver));
    setColour (juce::ComboBox::arrowColourId,             juce::Colour (kOrange));
    setColour (juce::PopupMenu::backgroundColourId,       juce::Colour (kMetalDark));
    setColour (juce::PopupMenu::textColourId,             juce::Colour (kSilver));
    setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (kBloodRed));
    setColour (juce::PopupMenu::highlightedTextColourId,  juce::Colours::white);
}

void RazorLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                          int x, int y, int width, int height,
                                          float sliderPos,
                                          float rotaryStartAngle, float rotaryEndAngle,
                                          juce::Slider& /*slider*/)
{
    const float cx = x + width  * 0.5f;
    const float cy = y + height * 0.5f;
    const float r  = juce::jmin (width, height) * 0.44f;

    // ── Outer ring (metal) ─────────────────────────────────────────────
    {
        juce::ColourGradient grad (juce::Colour (kMetalLight), cx - r, cy - r,
                                   juce::Colour (kMetalDark),  cx + r, cy + r, false);
        g.setGradientFill (grad);
        g.fillEllipse (cx - r, cy - r, r * 2.0f, r * 2.0f);

        g.setColour (juce::Colour (kMetalMid));
        g.drawEllipse (cx - r, cy - r, r * 2.0f, r * 2.0f, 2.0f);
    }

    // ── Teeth (gear-like notches) ──────────────────────────────────────
    {
        const int numTeeth = 16;
        const float toothR1 = r;
        const float toothR2 = r + 4.0f;
        g.setColour (juce::Colour (kMetalMid));
        for (int i = 0; i < numTeeth; ++i)
        {
            float a1 = juce::MathConstants<float>::twoPi * i / numTeeth;
            float a2 = a1 + juce::MathConstants<float>::twoPi / numTeeth * 0.5f;
            juce::Path tooth;
            tooth.addLineSegment ({ cx + std::cos(a1)*toothR1, cy + std::sin(a1)*toothR1,
                                    cx + std::cos(a1)*toothR2, cy + std::sin(a1)*toothR2 }, 3.0f);
            tooth.addLineSegment ({ cx + std::cos(a2)*toothR1, cy + std::sin(a2)*toothR1,
                                    cx + std::cos(a2)*toothR2, cy + std::sin(a2)*toothR2 }, 3.0f);
            g.strokePath (tooth, juce::PathStrokeType (2.5f));
        }
    }

    // ── Inner knob body ────────────────────────────────────────────────
    const float ir = r * 0.78f;
    {
        juce::ColourGradient grad (juce::Colour (0xFF303030), cx - ir*0.5f, cy - ir*0.5f,
                                   juce::Colour (0xFF101010), cx + ir*0.5f, cy + ir*0.5f, false);
        g.setGradientFill (grad);
        g.fillEllipse (cx - ir, cy - ir, ir * 2.0f, ir * 2.0f);
    }

    // ── Arc track ─────────────────────────────────────────────────────
    {
        const float trackR = r + 10.0f;
        juce::Path track;
        track.addArc (cx - trackR, cy - trackR, trackR*2.0f, trackR*2.0f,
                      rotaryStartAngle, rotaryEndAngle, true);
        g.setColour (juce::Colour (kMetalMid));
        g.strokePath (track, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved,
                                                    juce::PathStrokeType::rounded));
    }

    // ── Filled arc (orange glow) ────────────────────────────────────────
    {
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        const float trackR = r + 10.0f;
        juce::Path fill;
        fill.addArc (cx - trackR, cy - trackR, trackR*2.0f, trackR*2.0f,
                     rotaryStartAngle, angle, true);
        g.setColour (juce::Colour (kOrange));
        g.strokePath (fill, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));

        // Glow
        g.setColour (juce::Colour (kOrange).withAlpha (0.25f));
        g.strokePath (fill, juce::PathStrokeType (7.0f, juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));
    }

    // ── Pointer line ───────────────────────────────────────────────────
    {
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle)
                            - juce::MathConstants<float>::halfPi;
        juce::Path pointer;
        pointer.startNewSubPath (cx, cy);
        pointer.lineTo (cx + std::cos(angle) * ir * 0.85f,
                        cy + std::sin(angle) * ir * 0.85f);
        g.setColour (juce::Colour (kGlowRed));
        g.strokePath (pointer, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved,
                                                      juce::PathStrokeType::rounded));
        // Highlight tip
        g.setColour (juce::Colours::white.withAlpha (0.8f));
        g.fillEllipse (cx + std::cos(angle)*ir*0.8f - 2.5f,
                       cy + std::sin(angle)*ir*0.8f - 2.5f, 5.0f, 5.0f);
    }

    // ── Center screw dot ──────────────────────────────────────────────
    g.setColour (juce::Colour (kMetalLight));
    g.fillEllipse (cx - 4.0f, cy - 4.0f, 8.0f, 8.0f);
    g.setColour (juce::Colour (kMetalMid));
    g.drawEllipse (cx - 4.0f, cy - 4.0f, 8.0f, 8.0f, 1.5f);
}

void RazorLookAndFeel::drawComboBox (juce::Graphics& g, int w, int h,
                                      bool, int, int, int, int, juce::ComboBox& cb)
{
    juce::Rectangle<float> r (0.0f, 0.0f, (float)w, (float)h);

    // Dark panel
    g.setColour (juce::Colour (kMetalDark));
    g.fillRoundedRectangle (r, 3.0f);

    // Blood-red border
    g.setColour (juce::Colour (kBloodRed));
    g.drawRoundedRectangle (r.reduced (0.5f), 3.0f, 1.5f);

    // Arrow
    const float arrowCx = w - 14.0f;
    const float arrowCy = h * 0.5f;
    juce::Path arrow;
    arrow.addTriangle (arrowCx - 4.0f, arrowCy - 3.0f,
                       arrowCx + 4.0f, arrowCy - 3.0f,
                       arrowCx,        arrowCy + 3.0f);
    g.setColour (juce::Colour (kOrange));
    g.fillPath (arrow);
}

void RazorLookAndFeel::drawPopupMenuBackground (juce::Graphics& g, int w, int h)
{
    g.setColour (juce::Colour (kMetalDark));
    g.fillAll();
    g.setColour (juce::Colour (kBloodRed));
    g.drawRect (0, 0, w, h, 1);
}

void RazorLookAndFeel::drawPopupMenuItem (juce::Graphics& g,
                                           const juce::Rectangle<int>& area,
                                           bool isSeparator, bool isActive,
                                           bool isHighlighted, bool isTicked,
                                           bool, const juce::String& text,
                                           const juce::String&,
                                           const juce::Drawable*,
                                           const juce::Colour*)
{
    if (isSeparator)
    {
        g.setColour (juce::Colour (kBloodRed).withAlpha (0.5f));
        g.fillRect (area.getX() + 4, area.getCentreY(), area.getWidth() - 8, 1);
        return;
    }

    if (isHighlighted)
    {
        g.setColour (juce::Colour (kBloodRed));
        g.fillRect (area);
    }

    juce::Font f (juce::FontOptions (12.5f, juce::Font::bold));
    g.setFont (f);
    g.setColour (isHighlighted ? juce::Colours::white
                               : juce::Colour (kSilver));
    g.drawText (text, area.withTrimmedLeft (10), juce::Justification::centredLeft);

    if (isTicked)
    {
        g.setColour (juce::Colour (kOrange));
        juce::Path tick;
        tick.addStar ({ area.getRight() - 12.0f, area.getCentreY() }, 3.0f, 6.0f, 5);
        g.fillPath (tick);
    }
}

juce::Font RazorLookAndFeel::getComboBoxFont (juce::ComboBox&)
{
    return juce::Font (juce::FontOptions (12.0f, juce::Font::bold));
}

juce::Font RazorLookAndFeel::getPopupMenuFont()
{
    return juce::Font (juce::FontOptions (12.0f, juce::Font::bold));
}

// ═════════════════════════════════════════════════════════════════════════════
//  RazorKnob
// ═════════════════════════════════════════════════════════════════════════════
RazorKnob::RazorKnob (const juce::String& labelText)
    : name (labelText)
{
    slider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider.setRange (0.0, 1.0, 0.001);
    addAndMakeVisible (slider);

    valueLabel.setJustificationType (juce::Justification::centred);
    valueLabel.setColour (juce::Label::textColourId, juce::Colour (0xFFD0D0D0));
    valueLabel.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    addAndMakeVisible (valueLabel);

    slider.onValueChange = [this]
    {
        valueLabel.setText (juce::String ((int)(slider.getValue() * 100.0)) + "%",
                             juce::dontSendNotification);
    };
    valueLabel.setText ("0%", juce::dontSendNotification);
}

void RazorKnob::resized()
{
    const int h = getHeight();
    const int w = getWidth();
    slider.setBounds (0, 0, w, h - 16);
    valueLabel.setBounds (0, h - 16, w, 16);
}

void RazorKnob::paint (juce::Graphics& g)
{
    // Draw knob label below the value
    g.setColour (juce::Colour (0xFFBB3333));
    g.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    g.drawText (name, 0, getHeight() - 32, getWidth(), 16, juce::Justification::centred);
}

// ═════════════════════════════════════════════════════════════════════════════
//  WelcomeToRazorAudioProcessorEditor
// ═════════════════════════════════════════════════════════════════════════════
WelcomeToRazorAudioProcessorEditor::WelcomeToRazorAudioProcessorEditor
    (WelcomeToRazorAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&razorLAF);
    setSize (960, 520);

    // ── Knobs ─────────────────────────────────────────────────────────
    for (auto* k : { &knobGash, &knobBlood, &knobSharpen, &knobRecoil, &knobMixtape })
        addAndMakeVisible (k);

    // ── APVTS attachments ─────────────────────────────────────────────
    attGash    = std::make_unique<SliderAttachment> (p.apvts, "GASH",    knobGash.slider);
    attBlood   = std::make_unique<SliderAttachment> (p.apvts, "BLOOD",   knobBlood.slider);
    attSharpen = std::make_unique<SliderAttachment> (p.apvts, "SHARPEN", knobSharpen.slider);
    attRecoil  = std::make_unique<SliderAttachment> (p.apvts, "RECOIL",  knobRecoil.slider);
    attMixtape = std::make_unique<SliderAttachment> (p.apvts, "MIXTAPE", knobMixtape.slider);

    // ── Preset ComboBox ───────────────────────────────────────────────
    buildPresetBox();
    addAndMakeVisible (presetBox);

    // ── Animation ─────────────────────────────────────────────────────
    startTimerHz (30);
}

WelcomeToRazorAudioProcessorEditor::~WelcomeToRazorAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void WelcomeToRazorAudioProcessorEditor::buildPresetBox()
{
    presetBox.clear();
    for (int i = 0; i < kNumPresets; ++i)
        presetBox.addItem (kPresets[i].name, i + 1);

    presetBox.setSelectedId (processor.getCurrentProgram() + 1,
                              juce::dontSendNotification);

    presetBox.onChange = [this]
    {
        const int idx = presetBox.getSelectedId() - 1;
        if (idx >= 0)
            processor.setCurrentProgram (idx);
    };
}

void WelcomeToRazorAudioProcessorEditor::timerCallback()
{
    glowPhase += 0.06f;
    if (glowPhase > juce::MathConstants<float>::twoPi)
        glowPhase -= juce::MathConstants<float>::twoPi;

    repaint (0, 0, 320, 140); // Repaint only the title/glow area
}

// ─────────────────────────────────────────────────────────────────────────────
//  Layout
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessorEditor::resized()
{
    const int W = getWidth();
    const int H = getHeight();

    // Preset box at top-right
    presetBox.setBounds (W - 230, 120, 210, 180);

    // 5 knobs centred in lower portion
    const int knobW   = 100;
    const int knobH   = 110;
    const int knobY   = H - 160;
    const int totalKW = knobW * 5 + 20 * 4;
    int kx = (W - 230 - totalKW) / 2;

    for (auto* k : { &knobGash, &knobBlood, &knobSharpen, &knobRecoil, &knobMixtape })
    {
        k->setBounds (kx, knobY, knobW, knobH);
        kx += knobW + 20;
    }

    bgDirty = true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Paint
// ─────────────────────────────────────────────────────────────────────────────
void WelcomeToRazorAudioProcessorEditor::paint (juce::Graphics& g)
{
    paintBackground (g);
    paintDemonMask  (g);
    paintTitle      (g);
    paintRunicText  (g);
    paintPresetsPanel (g);
}

void WelcomeToRazorAudioProcessorEditor::paintBackground (juce::Graphics& g)
{
    const int W = getWidth();
    const int H = getHeight();

    // Base plate
    juce::ColourGradient bg (juce::Colour (0xFF0C0C0C), 0.0f, 0.0f,
                              juce::Colour (0xFF080808), (float)W, (float)H, false);
    g.setGradientFill (bg);
    g.fillAll();

    // Riveted panel border
    g.setColour (juce::Colour (0xFF1A1A1A));
    g.drawRect (0, 0, W, H, 4);
    g.setColour (juce::Colour (0xFF2A2A2A));
    g.drawRect (4, 4, W-8, H-8, 1);

    // Rivet screws at corners
    for (auto [rx, ry] : std::initializer_list<std::pair<int,int>> {
            {18,18}, {W-18,18}, {18,H-18}, {W-18,H-18} })
    {
        juce::ColourGradient cg (juce::Colour (0xFF505050), (float)rx-4, (float)ry-4,
                                  juce::Colour (0xFF202020), (float)rx+4, (float)ry+4, false);
        g.setGradientFill (cg);
        g.fillEllipse ((float)(rx-6), (float)(ry-6), 12.0f, 12.0f);
        g.setColour (juce::Colour (0xFF606060));
        g.drawEllipse ((float)(rx-6), (float)(ry-6), 12.0f, 12.0f, 1.0f);
        // Screw line
        g.setColour (juce::Colour (0xFF808080));
        g.drawLine ((float)rx-3, (float)ry, (float)rx+3, (float)ry, 1.0f);
    }

    // Blood spatter — scattered drops with RNG seeded for consistency
    juce::Random rng (0xDEADBEEF);
    g.saveState();

    // Splatter clusters
    paintBloodSplatter (g, rng,  80.0f, 250.0f, 45.0f, 12);
    paintBloodSplatter (g, rng, 200.0f, 380.0f, 30.0f,  8);
    paintBloodSplatter (g, rng, 600.0f, 200.0f, 25.0f,  7);
    paintBloodSplatter (g, rng, 750.0f, 440.0f, 35.0f, 10);
    paintBloodSplatter (g, rng, 420.0f, 320.0f, 20.0f,  5);

    g.restoreState();

    // Horizontal separator line
    const float sepY = 100.0f;
    juce::ColourGradient sep (juce::Colour (0xFF440000), 0.0f, sepY,
                               juce::Colour (0xFF000000), (float)W, sepY, true);
    g.setGradientFill (sep);
    g.fillRect (0.0f, sepY - 1.0f, (float)W, 2.0f);

    // Lower panel area
    g.setColour (juce::Colour (0xFF111111));
    g.fillRect (0, H - 180, W - 230, 180);
    g.setColour (juce::Colour (0xFF2A1A00));
    g.drawRect (0, H - 180, W - 230, 180, 1);
}

void WelcomeToRazorAudioProcessorEditor::paintBloodSplatter (
    juce::Graphics& g, juce::Random& rng,
    float cx, float cy, float maxR, int n)
{
    for (int i = 0; i < n; ++i)
    {
        const float angle = rng.nextFloat() * juce::MathConstants<float>::twoPi;
        const float dist  = rng.nextFloat() * maxR;
        const float r     = rng.nextFloat() * 5.0f + 1.0f;
        const float x     = cx + std::cos(angle) * dist;
        const float y     = cy + std::sin(angle) * dist;
        const float alpha = rng.nextFloat() * 0.5f + 0.1f;
        g.setColour (juce::Colour (0xFF8B0000).withAlpha (alpha));
        g.fillEllipse (x - r, y - r, r*2.0f, r*2.0f);

        // Drip streak
        if (rng.nextBool())
        {
            const float dripLen = rng.nextFloat() * 20.0f + 5.0f;
            g.setColour (juce::Colour (0xFF660000).withAlpha (alpha * 0.6f));
            g.fillRect (x - 1.0f, y, 2.0f, dripLen);
        }
    }
}

void WelcomeToRazorAudioProcessorEditor::paintTitle (juce::Graphics& g)
{
    const int W = getWidth();

    // Shadow/glow pulse
    const float glowA = 0.35f + 0.25f * std::sin (glowPhase);

    // Outer glow
    juce::ColourGradient glow (juce::Colour (0xFFCC0000).withAlpha (glowA),
                                W * 0.3f, 30.0f,
                                juce::Colours::transparentBlack,
                                W * 0.5f, 80.0f, true);
    g.setGradientFill (glow);
    g.fillRect (60, 5, W - 320, 90);

    // Title text
    juce::Font titleFont (juce::FontOptions (62.0f, juce::Font::bold | juce::Font::italic));
    g.setFont (titleFont);

    // Drop shadow
    g.setColour (juce::Colours::black.withAlpha (0.9f));
    g.drawText ("WELCOME TO RAZOR", 63, 18, W - 320, 68, juce::Justification::centred);

    // Main text — blood red gradient
    const juce::String title = "WELCOME TO RAZOR";
    juce::GlyphArrangement ga;
    ga.addFittedText (titleFont, title, 60.0f, 15.0f,
                      (float)(W - 320), 68.0f,
                      juce::Justification::centred, 1);

    juce::Path textPath;
    ga.createPath (textPath);

    juce::ColourGradient textGrad (juce::Colour (0xFFFF3030), 60.0f, 15.0f,
                                    juce::Colour (0xFF8B0000), 60.0f, 83.0f, false);
    g.setGradientFill (textGrad);
    g.fillPath (textPath);

    // Highlight edge
    g.setColour (juce::Colour (0xFFFF8888).withAlpha (0.4f));
    g.strokePath (textPath, juce::PathStrokeType (0.5f));
}

void WelcomeToRazorAudioProcessorEditor::paintRunicText (juce::Graphics& g)
{
    const int W = getWidth();
    const int H = getHeight();

    // Runic / latin cryptic band across the middle
    g.setFont (juce::Font (juce::FontOptions (13.0f, juce::Font::bold)));
    g.setColour (juce::Colour (0xFF883333).withAlpha (0.7f));

    const juce::String runic = "MENS  KNXEF  PRO  RAW  //  SSTRY  VOID  KRUMP  //  RAZOR  EDGE";
    g.drawText (runic, 20, H - 185, W - 250, 20, juce::Justification::centred);
}

void WelcomeToRazorAudioProcessorEditor::paintPresetsPanel (juce::Graphics& g)
{
    const int W = getWidth();

    // Preset list panel background
    juce::Rectangle<float> panel ((float)(W - 230), 108.0f, 218.0f, 200.0f);
    g.setColour (juce::Colour (0xFF0F0F0F));
    g.fillRoundedRectangle (panel, 4.0f);
    g.setColour (juce::Colour (0xFF550000));
    g.drawRoundedRectangle (panel.reduced (0.5f), 4.0f, 1.5f);

    // Header "PRESETS"
    g.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    g.setColour (juce::Colour (0xFF884444));
    g.drawText ("PRESETS", W - 228, 110, 210, 14, juce::Justification::centred);

    // Divider
    g.setColour (juce::Colour (0xFF440000));
    g.fillRect (W - 228, 124, 210, 1);
}

void WelcomeToRazorAudioProcessorEditor::paintDemonMask (juce::Graphics& g)
{
    // Stylised demon skull / face in the left area using primitives
    const float cx = 120.0f;
    const float cy = 55.0f;

    // Skull base
    juce::ColourGradient skulls (juce::Colour (0xFF1A1A1A), cx - 40, cy - 30,
                                  juce::Colour (0xFF050505), cx + 40, cy + 40, false);
    g.setGradientFill (skulls);
    g.fillEllipse (cx - 38, cy - 28, 76.0f, 68.0f);

    // Horns
    auto drawHorn = [&] (float hx, float hy, bool left)
    {
        juce::Path horn;
        if (left)
        {
            horn.startNewSubPath (hx, hy);
            horn.lineTo (hx - 18, hy - 28);
            horn.lineTo (hx - 6,  hy - 5);
        }
        else
        {
            horn.startNewSubPath (hx, hy);
            horn.lineTo (hx + 18, hy - 28);
            horn.lineTo (hx + 6,  hy - 5);
        }
        horn.closeSubPath();
        juce::ColourGradient hg (juce::Colour (0xFF2A2A2A), hx, hy - 28,
                                  juce::Colour (0xFF101010), hx, hy, false);
        g.setGradientFill (hg);
        g.fillPath (horn);
        g.setColour (juce::Colour (0xFF330000));
        g.strokePath (horn, juce::PathStrokeType (1.0f));
    };

    drawHorn (cx - 28, cy - 22, true);
    drawHorn (cx + 28, cy - 22, false);

    // Eyes — glowing orange
    const float eyeGlow = 0.6f + 0.4f * std::sin (glowPhase * 1.3f);
    for (int side : {-1, 1})
    {
        const float ex = cx + side * 15.0f;
        const float ey = cy - 4.0f;

        // Glow halo
        juce::ColourGradient eg (juce::Colour (0xFFFF4400).withAlpha (eyeGlow * 0.6f),
                                  ex, ey,
                                  juce::Colours::transparentBlack,
                                  ex + 18, ey, true);
        g.setGradientFill (eg);
        g.fillEllipse (ex - 15, ey - 12, 30.0f, 24.0f);

        // Eye core
        g.setColour (juce::Colour (0xFFFF6600).withAlpha (eyeGlow));
        g.fillEllipse (ex - 6, ey - 5, 12.0f, 10.0f);
        g.setColour (juce::Colour (0xFFFFAA00).withAlpha (eyeGlow));
        g.fillEllipse (ex - 3, ey - 2.5f, 6.0f, 5.0f);
    }

    // Mouth / teeth
    g.setColour (juce::Colour (0xFF080808));
    g.fillRect  (cx - 18.0f, cy + 14.0f, 36.0f, 10.0f);
    g.setColour (juce::Colour (0xFFB0B0B0).withAlpha (0.8f));
    for (int t = 0; t < 6; ++t)
    {
        const float tx = cx - 15.0f + t * 6.0f;
        juce::Path tooth;
        tooth.addTriangle (tx, cy + 14.0f, tx + 5.0f, cy + 14.0f, tx + 2.5f, cy + 20.0f);
        g.fillPath (tooth);
    }

    // Runic symbol on forehead
    g.setColour (juce::Colour (0xFF660000).withAlpha (0.7f));
    g.setFont (juce::Font (juce::FontOptions (14.0f, juce::Font::bold)));
    g.drawText ("X", (int)(cx - 8), (int)(cy - 24), 16, 16, juce::Justification::centred);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Factory
// ─────────────────────────────────────────────────────────────────────────────
juce::AudioProcessorEditor* WelcomeToRazorAudioProcessor::createEditor()
{
    return new WelcomeToRazorAudioProcessorEditor (*this);
}
