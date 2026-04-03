cmake_minimum_required(VERSION 3.22)
project(Vayu VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE installed to /usr/local from the build
set(JUCE_DIR "/usr/local/lib/cmake/JUCE-8.0.4" CACHE PATH "JUCE CMake directory")

find_package(JUCE CONFIG REQUIRED)

add_executable(Vayu
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_link_libraries(Vayu PRIVATE JUCE::juce_audio_plugin_client)
