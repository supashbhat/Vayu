//
//  AudioEQAudioProcessorEditor.swift
//  AudioEQ
//
//  Created by Supash Bhat on 4/2/26.
//


#pragma once

#include <JuceHeader.h>

// Forward declarations to keep the header clean
class AudioEQAudioProcessorEditor;

// ==============================================================================
// Lock-Free FIFO for Spectrum Analyzer (Replaces SpinLock)
// ==============================================================================
template<typename T>
class LockFreeQueue
{
public:
    LockFreeQueue(int capacity) : fifo(capacity), buffer(static_cast<size_t>(capacity)) {}
    
    void push(T item) {
        auto writeHandle = fifo.write(1);
        if (writeHandle.blockSize1 > 0)
            buffer[static_cast<size_t>(writeHandle.startIndex1)] = item;
    }
    
    bool pop(T& item) {
        auto readHandle = fifo.read(1);
        if (readHandle.blockSize1 > 0) {
            item = buffer[static_cast<size_t>(readHandle.startIndex1)];
            return true;
        }
        return false;
    }

private:
    juce::AbstractFifo fifo;
    std::vector<T> buffer;
};

// ==============================================================================
class AudioEQAudioProcessor : public juce::AudioProcessor,
                              private juce::AudioProcessorValueTreeState::Listener
{
public:
    enum class EffectSlot { distortion = 0, chorus, flanger, phaser, delay, reverb };

    AudioEQAudioProcessor();
    ~AudioEQAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Vayu"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override;

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Preset Management
    void applyPreset(int presetIndex);
    int getCurrentPresetIndex() const noexcept;
    juce::StringArray getPresetNames() const;
    
    void applyRackPreset(int presetIndex);
    int getCurrentRackPresetIndex() const noexcept;
    juce::StringArray getRackPresetNames() const;
    
    bool saveUserRackPreset(const juce::String& name);
    bool loadUserRackPreset(const juce::String& name);
    juce::StringArray getUserRackPresetNames() const;

    // Modular Chain Management (Atomic for thread safety)
    void setEffectOrder(const std::vector<int>& newOrder);
    
    // UI Helpers
    void copySpectrumData(std::array<float, 128>& dest);
    float getMagnitudeResponseAtFrequency(float frequency);

    juce::AudioProcessorValueTreeState parameters;

private:
    void updateProcessingState();
    void parameterChanged(const juce::String& parameterId, float newValue) override;
    
    // Setup APVTS Layout
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // --- DSP Modules ---
    // Oversampling to prevent aliasing in the distortion phase
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    
    // Using JUCE's native DSP modules for cleaner, highly optimized processing
    juce::dsp::ProcessorChain<juce::dsp::IIR::Filter<float>, // Low Shelf
                              juce::dsp::IIR::Filter<float>, // Low Mid
                              juce::dsp::IIR::Filter<float>, // Mid
                              juce::dsp::IIR::Filter<float>, // High Mid
                              juce::dsp::IIR::Filter<float>> // High Shelf
                              eqChain;

    juce::dsp::Chorus<float> chorus;
    juce::dsp::Phaser<float> phaser;
    juce::dsp::Reverb reverb;
    
    // Custom Delay and Distortion Processors (Moved to isolated methods for modularity)
    void processDistortion(juce::AudioBuffer<float>& buffer, int channelCount);
    void processDelay(juce::AudioBuffer<float>& buffer, int channelCount);
    void processChorusFlangerPhaser(juce::AudioBuffer<float>& buffer, int channelCount, EffectSlot slot);

    // Dynamic Effect Routing (Thread-safe reading via std::atomic)
    std::atomic<int> effectOrder[6] { 0, 1, 2, 3, 4, 5 };

    // Smoothed variables to prevent zipper noise
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> outputGainSmoother;

    // Spectrum Analyzer (Lock-Free)
    static constexpr int fftOrder = 11;
    static constexpr int fftSize = 1 << fftOrder;
    static constexpr int spectrumBins = 128;
    
    juce::dsp::FFT analyserFft { fftOrder };
    juce::dsp::WindowingFunction<float> analyserWindow { fftSize, juce::dsp::WindowingFunction<float>::hann };
    
    LockFreeQueue<float> audioToUiQueue { fftSize * 4 }; // Lock-free queue for analyzer
    std::array<float, fftSize * 2> analyserData {};
    std::array<float, spectrumBins> spectrumData {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEQAudioProcessor)
};