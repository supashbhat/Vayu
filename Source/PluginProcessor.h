#pragma once

#include <JuceHeader.h>

class AudioEQAudioProcessor : public juce::AudioProcessor,
                              private juce::AudioProcessorValueTreeState::Listener
{
public:
    enum class EffectSlot
    {
        distortion = 0,
        chorus,
        flanger,
        phaser,
        delay,
        stereo,
        reverb,
        compressor
    };

    AudioEQAudioProcessor();
    ~AudioEQAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void applyPreset(int presetIndex);
    int getCurrentPresetIndex() const noexcept;
    juce::StringArray getPresetNames() const;
    juce::StringArray getRackPresetNames() const;
    void applyRackPreset(int presetIndex);
    int getCurrentRackPresetIndex() const noexcept;
    juce::StringArray getEffectPresetNames(EffectSlot effectSlot) const;
    void applyEffectPreset(EffectSlot effectSlot, int presetIndex);
    juce::StringArray getUserRackPresetNames() const;
    bool saveUserRackPreset(const juce::String& presetName);
    bool loadUserRackPreset(const juce::String& presetName);
    bool deleteUserRackPreset(const juce::String& presetName);
    juce::StringArray getUserEffectPresetNames(EffectSlot effectSlot) const;
    bool saveUserEffectPreset(EffectSlot effectSlot, const juce::String& presetName);
    bool loadUserEffectPreset(EffectSlot effectSlot, const juce::String& presetName);
    bool deleteUserEffectPreset(EffectSlot effectSlot, const juce::String& presetName);
    float getMagnitudeResponseAtFrequency(double frequency) const;
    float getInputLevel() const noexcept;
    float getOutputLevel() const noexcept;
    double getCurrentSampleRateValue() const noexcept;
    void copySpectrumData(std::array<float, 128>& destination) const;
    float getCompressorGainReductionDb() const noexcept;
    bool canUndo() const noexcept;
    bool canRedo() const noexcept;
    void undoLastAction();
    void redoLastAction();

    juce::AudioProcessorValueTreeState parameters;

    // Effect order management (thread-safe)
    void setEffectOrder(const std::array<int, 8>& newOrder);
    std::array<int, 8> getEffectOrder() const;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using Coefficients = juce::dsp::IIR::Coefficients<float>;
    using OutputGain = juce::dsp::Gain<float>;
    using MonoChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter, Filter, OutputGain>;

    static constexpr size_t stereoChannels = 2;
    static constexpr size_t effectCount = 8;
    static constexpr size_t parameterListenerCount = 56;
    static constexpr int spectrumBins = 128;
    static constexpr int fftOrder = 11;
    static constexpr int fftSize = 1 << fftOrder;

    std::array<MonoChain, stereoChannels> filterChains;
    std::array<juce::dsp::Chorus<float>, stereoChannels> flangerProcessors;
    juce::AudioBuffer<float> modulationScratchBuffer;
    std::array<std::vector<float>, stereoChannels> chorusDelayBuffers;
    std::array<std::vector<float>, stereoChannels> delayBuffers;
    std::array<float, stereoChannels> delayFilterState {};
    std::array<float, stereoChannels> distortionToneState {};
    std::array<float, stereoChannels> phaserFeedbackState {};
    std::array<std::array<std::array<float, 2>, 6>, stereoChannels> phaserStageState {};
    float compressorEnvelope = 0.0f;
    float compressorRmsState = 0.0f;
    float compressorGainState = 1.0f;
    float compressorSidechainLowpassState = 0.0f;
    std::atomic<float> compressorGainReductionDb { 0.0f };
    juce::Reverb reverbProcessor;
    juce::Reverb::Parameters reverbParameters;
    float chorusPhase = 0.0f;
    float flangerPhase = 0.0f;
    float phaserPhase = 0.0f;
    float stereoPhase = 0.0f;
    int chorusWritePosition = 0;
    int delayWritePosition = 0;
    int maxDelaySamples = 1;
    double currentSampleRate = 44100.0;
    int currentPresetIndex = 0;
    int currentRackPresetIndex = 0;
    std::atomic<bool> processingStateNeedsUpdate { true };
    std::atomic<float> inputLevel { 0.0f };
    std::atomic<float> outputLevel { 0.0f };
    mutable juce::SpinLock responseCurveLock;
    std::array<Coefficients::Ptr, 5> responseCurveCoefficients;
    float responseOutputGain = 1.0f;
    juce::dsp::FFT analyserFft { fftOrder };
    juce::dsp::WindowingFunction<float> analyserWindow { fftSize, juce::dsp::WindowingFunction<float>::hann };
    std::array<float, fftSize> analyserFifo {};
    std::array<float, fftSize * 2> analyserData {};
    std::array<float, spectrumBins> spectrumData {};
    int analyserFifoIndex = 0;
    mutable juce::SpinLock analyserLock;
    juce::UndoManager undoManager;

    // Effect order storage (atomic for thread-safety)
    std::array<std::atomic<int>, effectCount> effectOrder;

    void updateProcessingState();
    void parameterChanged(const juce::String& parameterId, float newValue) override;
    static float measurePeakLevel(const juce::AudioBuffer<float>& buffer, int channelCount);
    void pushNextAnalyserSample(float sample) noexcept;
    void generateSpectrumData();
    
    // Effect processing functions
    void processDistortion(juce::AudioBuffer<float>& buffer, int channelCount);
    void processChorus(juce::AudioBuffer<float>& buffer, int channelCount);
    void processFlanger(juce::AudioBuffer<float>& buffer, int channelCount);
    void processPhaser(juce::AudioBuffer<float>& buffer, int channelCount);
    void processDelay(juce::AudioBuffer<float>& buffer, int channelCount);
    void processStereo(juce::AudioBuffer<float>& buffer, int channelCount);
    void processReverb(juce::AudioBuffer<float>& buffer, int channelCount);
    void processCompressor(juce::AudioBuffer<float>& buffer,
                           int channelCount,
                           const juce::AudioBuffer<float>* sidechainBuffer = nullptr);
    
    // Master effect processor that respects order
    void processEffectsInOrder(juce::AudioBuffer<float>& buffer,
                               int channelCount,
                               const juce::AudioBuffer<float>* sidechainBuffer);

    static float getParameterValue(const juce::AudioProcessorValueTreeState& state,
                                   const juce::String& parameterId);
    static void setParameterValue(juce::AudioProcessorValueTreeState& state,
                                  const juce::String& parameterId,
                                  float newValue);
    static juce::File getUserPresetRootDirectory();
    static juce::File getUserRackPresetDirectory();
    static juce::File getUserEffectPresetDirectory(EffectSlot effectSlot);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEQAudioProcessor)
};
