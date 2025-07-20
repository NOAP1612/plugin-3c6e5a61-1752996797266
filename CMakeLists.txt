CMakeLists.txt

cmake_minimum_required(VERSION 3.15)

project(VintageCompressor VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# JUCE setup
set(JUCE_DIR "$ENV{JUCE_DIR}")
if(NOT JUCE_DIR)
    message(FATAL_ERROR "Please set JUCE_DIR environment variable to your JUCE path")
endif()
add_subdirectory(${JUCE_DIR} JUCE)

juce_add_plugin(VintageCompressor
    COMPANY_NAME "YourCompany"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE Vcpr
    FORMATS VST3 AU
    PRODUCT_NAME "VintageCompressor"
)

target_sources(VintageCompressor
    PRIVATE
        PluginProcessor.cpp
        PluginProcessor.h
        PluginEditor.cpp
        PluginEditor.h
)

target_compile_definitions(VintageCompressor
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(VintageCompressor
    PRIVATE
        juce::juce_audio_utils
)
```