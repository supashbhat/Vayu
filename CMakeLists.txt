cmake_minimum_required(VERSION 3.22)

project(Vayu VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(JUCE_DIR "" CACHE PATH "Path to a JUCE source checkout or installed JUCE CMake package")

if(JUCE_DIR AND EXISTS "${JUCE_DIR}/CMakeLists.txt")
    add_subdirectory("${JUCE_DIR}" JUCE)
elseif(JUCE_DIR AND EXISTS "${JUCE_DIR}/JUCEConfig.cmake")
    list(PREPEND CMAKE_PREFIX_PATH "${JUCE_DIR}")
    find_package(JUCE CONFIG REQUIRED)
else()
    find_package(JUCE CONFIG REQUIRED)
endif()

set(VAYU_FORMATS Standalone VST3)
if(APPLE)
    list(APPEND VAYU_FORMATS AU)
endif()

juce_add_plugin(Vayu
    COMPANY_NAME "Supash Bhat"
    BUNDLE_ID com.supash.vayu
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Sphb
    PLUGIN_CODE Aeq1
    FORMATS ${VAYU_FORMATS}
    PRODUCT_NAME "Vayu"
)

juce_generate_juce_header(Vayu)

target_sources(Vayu
    PRIVATE
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
)

target_compile_definitions(Vayu
    PUBLIC
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(Vayu
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_osc
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
