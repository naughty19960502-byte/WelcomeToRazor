cmake_minimum_required(VERSION 3.22)

project(WelcomeToRazor VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── JUCE 8 via FetchContent ────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# ── Plugin target ──────────────────────────────────────────────────────────────
juce_add_plugin(WelcomeToRazor
    COMPANY_NAME            "RazorAudio"
    PLUGIN_MANUFACTURER_CODE Razr
    PLUGIN_CODE             Wtr1
    FORMATS                 VST3 AU Standalone
    PRODUCT_NAME            "WelcomeToRazor"

    IS_SYNTH                FALSE
    NEEDS_MIDI_INPUT        FALSE
    NEEDS_MIDI_OUTPUT       FALSE
    IS_MIDI_EFFECT          FALSE

    EDITOR_WANTS_KEYBOARD_FOCUS FALSE

    VST3_CATEGORIES         "Fx" "Distortion" "Dynamics"
    AU_MAIN_TYPE            kAudioUnitType_Effect

    COPY_PLUGIN_AFTER_BUILD FALSE
)

# ── Sources ────────────────────────────────────────────────────────────────────
target_sources(WelcomeToRazor
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

# ── Compile definitions ────────────────────────────────────────────────────────
target_compile_definitions(WelcomeToRazor
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

# ── JUCE modules ──────────────────────────────────────────────────────────────
target_link_libraries(WelcomeToRazor
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_graphics
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
