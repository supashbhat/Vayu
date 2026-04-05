#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    using Parameter = juce::AudioParameterFloat;

    constexpr auto stateId = "VayuState";
    constexpr auto selectedPresetProperty = "selectedPreset";
    constexpr auto selectedRackPresetProperty = "selectedRackPreset";
    constexpr auto effectOrderPropertyPrefix = "effectOrder";

    constexpr const char* lowShelfFreqId = "lowShelfFreq";
    constexpr const char* lowShelfGainId = "lowShelfGain";
    constexpr const char* lowShelfQId = "lowShelfQ";
    constexpr const char* lowMidFreqId = "lowMidFreq";
    constexpr const char* lowMidGainId = "lowMidGain";
    constexpr const char* lowMidQId = "lowMidQ";
    constexpr const char* midFreqId = "midFreq";
    constexpr const char* midGainId = "midGain";
    constexpr const char* midQId = "midQ";
    constexpr const char* highMidFreqId = "highMidFreq";
    constexpr const char* highMidGainId = "highMidGain";
    constexpr const char* highMidQId = "highMidQ";
    constexpr const char* highShelfFreqId = "highShelfFreq";
    constexpr const char* highShelfGainId = "highShelfGain";
    constexpr const char* highShelfQId = "highShelfQ";
    constexpr const char* outputGainId = "outputGain";
    constexpr const char* distortionEnabledId = "distortionEnabled";
    constexpr const char* distortionModeId = "distortionMode";
    constexpr const char* distortionDriveId = "distortionDrive";
    constexpr const char* distortionToneId = "distortionTone";
    constexpr const char* distortionMixId = "distortionMix";
    constexpr const char* chorusEnabledId = "chorusEnabled";
    constexpr const char* chorusModeId = "chorusMode";
    constexpr const char* chorusRateId = "chorusRate";
    constexpr const char* chorusDepthId = "chorusDepth";
    constexpr const char* chorusMixId = "chorusMix";
    constexpr const char* flangerEnabledId = "flangerEnabled";
    constexpr const char* flangerModeId = "flangerMode";
    constexpr const char* flangerRateId = "flangerRate";
    constexpr const char* flangerDepthId = "flangerDepth";
    constexpr const char* flangerMixId = "flangerMix";
    constexpr const char* phaserEnabledId = "phaserEnabled";
    constexpr const char* phaserModeId = "phaserMode";
    constexpr const char* phaserRateId = "phaserRate";
    constexpr const char* phaserDepthId = "phaserDepth";
    constexpr const char* phaserMixId = "phaserMix";
    constexpr const char* delayEnabledId = "delayEnabled";
    constexpr const char* delayModeId = "delayMode";
    constexpr const char* delayTimeId = "delayTime";
    constexpr const char* delayFeedbackId = "delayFeedback";
    constexpr const char* delayMixId = "delayMix";
    constexpr const char* reverbEnabledId = "reverbEnabled";
    constexpr const char* reverbModeId = "reverbMode";
    constexpr const char* reverbSizeId = "reverbSize";
    constexpr const char* reverbDampingId = "reverbDamping";
    constexpr const char* reverbMixId = "reverbMix";
    constexpr const char* stereoEnabledId = "stereoEnabled";
    constexpr const char* stereoModeId = "stereoMode";
    constexpr const char* stereoRateId = "stereoRate";
    constexpr const char* stereoDepthId = "stereoDepth";
    constexpr const char* stereoMixId = "stereoMix";
    constexpr const char* compressorEnabledId = "compressorEnabled";
    constexpr const char* compressorModeId = "compressorMode";
    constexpr const char* compressorThresholdId = "compressorThreshold";
    constexpr const char* compressorAmountId = "compressorAmount";
    constexpr const char* compressorMixId = "compressorMix";

    constexpr int effectSlotCount = 8;

    using Coefficients = juce::dsp::IIR::Coefficients<float>;

    struct PresetDefinition
    {
        const char* name;
        float lowShelfFreq, lowShelfGain, lowShelfQ;
        float lowMidFreq, lowMidGain, lowMidQ;
        float midFreq, midGain, midQ;
        float highMidFreq, highMidGain, highMidQ;
        float highShelfFreq, highShelfGain, highShelfQ;
        float outputGain;
    };

    struct RackPresetDefinition
    {
        const char* name;
        float distortionEnabled, distortionMode, distortionDrive, distortionTone, distortionMix;
        float chorusEnabled, chorusMode, chorusRate, chorusDepth, chorusMix;
        float flangerEnabled, flangerMode, flangerRate, flangerDepth, flangerMix;
        float phaserEnabled, phaserMode, phaserRate, phaserDepth, phaserMix;
        float delayEnabled, delayMode, delayTime, delayFeedback, delayMix;
        float stereoEnabled, stereoMode, stereoRate, stereoDepth, stereoMix;
        float reverbEnabled, reverbMode, reverbSize, reverbDamping, reverbMix;
        float compressorEnabled, compressorMode, compressorThreshold, compressorAmount, compressorMix;
        std::array<int, effectSlotCount> effectOrder;
    };

    const auto& getPresetDefinitions()
    {
        static const std::array<PresetDefinition, 7> presets{{
            { "Flat", 110.0f, 0.0f, 0.75f, 260.0f, 0.0f, 0.90f, 1000.0f, 0.0f, 1.00f, 3600.0f, 0.0f, 0.90f, 9200.0f, 0.0f, 0.75f, 0.0f },
            { "Bass Boost", 90.0f, 5.5f, 0.70f, 220.0f, 2.5f, 0.85f, 950.0f, -1.0f, 1.10f, 3400.0f, 1.0f, 0.90f, 9600.0f, 1.8f, 0.70f, -1.0f },
            { "Vocal Presence", 120.0f, -2.0f, 0.75f, 320.0f, -0.8f, 0.95f, 1800.0f, 2.8f, 0.85f, 4200.0f, 3.2f, 0.80f, 10000.0f, 1.2f, 0.70f, 0.0f },
            { "Air Lift", 130.0f, -1.0f, 0.75f, 300.0f, 0.0f, 0.90f, 1400.0f, 0.5f, 0.95f, 5200.0f, 2.4f, 0.75f, 11500.0f, 5.0f, 0.60f, -0.5f },
            { "High Pass", 180.0f, -18.0f, 0.65f, 320.0f, -6.0f, 1.10f, 1200.0f, -1.5f, 1.00f, 4200.0f, 0.0f, 0.90f, 11000.0f, 0.0f, 0.70f, 1.0f },
            { "Low Pass", 110.0f, 0.0f, 0.75f, 280.0f, 0.0f, 0.90f, 900.0f, -1.0f, 0.95f, 3200.0f, -5.0f, 1.00f, 4200.0f, -18.0f, 0.65f, 1.0f },
            { "Smile Curve", 85.0f, 4.0f, 0.70f, 240.0f, -2.2f, 1.00f, 1100.0f, -1.5f, 0.95f, 4200.0f, 2.5f, 0.85f, 10800.0f, 4.5f, 0.65f, -0.8f }
        }};
        return presets;
    }

    const auto& getRackPresetDefinitions()
    {
        static const std::array<RackPresetDefinition, 8> presets{{
            { "Clean Slate", 0, 0, 6.0f, 0.65f, 0.35f, 0, 0, 0.8f, 0.45f, 0.30f, 0, 0, 0.35f, 0.55f, 0.25f, 0, 0, 0.4f, 0.55f, 0.30f, 0, 0, 0.32f, 0.35f, 0.25f, 0, 0, 0.45f, 0.55f, 0.40f, 0, 0, 0.45f, 0.35f, 0.22f, 0, 0, -18.0f, 0.40f, 1.0f, {{ 0, 7, 1, 2, 3, 4, 5, 6 }} },
            { "Solar Haze", 1, 1, 16.0f, 0.58f, 0.48f, 1, 2, 1.6f, 0.78f, 0.56f, 0, 0, 0.35f, 0.55f, 0.25f, 0, 0, 0.4f, 0.55f, 0.30f, 1, 1, 0.42f, 0.46f, 0.34f, 1, 0, 0.22f, 0.48f, 0.26f, 1, 2, 0.52f, 0.22f, 0.38f, 1, 1, -19.0f, 0.46f, 0.72f, {{ 0, 7, 1, 4, 5, 6, 2, 3 }} },
            { "Neon Tunnel", 0, 0, 6.0f, 0.65f, 0.35f, 0, 0, 0.8f, 0.45f, 0.30f, 1, 2, 0.95f, 0.86f, 0.62f, 1, 2, 1.1f, 0.74f, 0.46f, 1, 2, 0.24f, 0.62f, 0.42f, 1, 2, 0.68f, 0.82f, 0.58f, 1, 1, 0.58f, 0.28f, 0.30f, 1, 3, -25.0f, 0.78f, 0.62f, {{ 7, 2, 3, 4, 5, 6, 1, 0 }} },
            { "Cathedral Bloom", 0, 0, 6.0f, 0.65f, 0.35f, 1, 1, 0.42f, 0.52f, 0.24f, 0, 0, 0.35f, 0.55f, 0.25f, 0, 0, 0.4f, 0.55f, 0.30f, 1, 0, 0.66f, 0.52f, 0.18f, 1, 0, 0.16f, 0.42f, 0.20f, 1, 1, 0.88f, 0.18f, 0.54f, 1, 1, -22.0f, 0.34f, 0.55f, {{ 7, 1, 4, 5, 6, 3, 2, 0 }} },
            { "Broken Cassette", 1, 2, 22.0f, 0.34f, 0.62f, 0, 0, 0.8f, 0.45f, 0.30f, 1, 1, 0.22f, 0.72f, 0.38f, 0, 0, 0.4f, 0.55f, 0.30f, 1, 1, 0.38f, 0.55f, 0.36f, 1, 1, 0.28f, 0.58f, 0.24f, 1, 0, 0.38f, 0.72f, 0.24f, 1, 2, -18.0f, 0.58f, 0.78f, {{ 0, 7, 4, 2, 5, 6, 1, 3 }} },
            { "Glass Drift", 0, 0, 6.0f, 0.65f, 0.35f, 1, 0, 0.28f, 0.64f, 0.34f, 1, 1, 0.34f, 0.58f, 0.28f, 1, 0, 0.26f, 0.48f, 0.18f, 1, 0, 0.18f, 0.28f, 0.16f, 1, 2, 0.34f, 0.64f, 0.32f, 1, 2, 0.36f, 0.14f, 0.30f, 1, 0, -21.0f, 0.36f, 0.48f, {{ 7, 1, 2, 3, 5, 4, 6, 0 }} },
            { "Blackout Pulse", 1, 2, 24.0f, 0.72f, 0.68f, 0, 0, 0.8f, 0.45f, 0.30f, 0, 0, 0.35f, 0.55f, 0.25f, 1, 1, 0.85f, 0.82f, 0.58f, 1, 2, 0.16f, 0.74f, 0.46f, 1, 1, 0.52f, 0.78f, 0.44f, 0, 0, 0.45f, 0.35f, 0.22f, 1, 3, -29.0f, 0.86f, 0.82f, {{ 0, 7, 3, 4, 5, 1, 6, 2 }} },
            { "Dreamline", 0, 0, 6.0f, 0.65f, 0.35f, 1, 2, 0.62f, 0.86f, 0.58f, 0, 0, 0.35f, 0.55f, 0.25f, 0, 0, 0.4f, 0.55f, 0.30f, 1, 0, 0.48f, 0.32f, 0.28f, 1, 0, 0.20f, 0.70f, 0.32f, 1, 2, 0.74f, 0.12f, 0.44f, 1, 1, -20.0f, 0.42f, 0.60f, {{ 7, 1, 4, 5, 6, 2, 3, 0 }} }
        }};
        return presets;
    }

    juce::NormalisableRange<float> makeFrequencyRange(float start, float end, float skew)
    {
        return { start, end, 1.0f, skew };
    }

    juce::StringArray buildPresetNames()
    {
        juce::StringArray names;
        for (const auto& preset : getPresetDefinitions())
            names.add(preset.name);
        return names;
    }

    juce::StringArray buildRackPresetNames()
    {
        juce::StringArray names;
        for (const auto& preset : getRackPresetDefinitions())
            names.add(preset.name);
        return names;
    }

    constexpr float twoPi = juce::MathConstants<float>::twoPi;

    float wrapPhase(float phase)
    {
        while (phase >= twoPi)
            phase -= twoPi;
        while (phase < 0.0f)
            phase += twoPi;
        return phase;
    }

    void animateStereoField(juce::AudioBuffer<float>& wetBuffer,
                            int channelCount,
                            float& phase,
                            double sampleRate,
                            float rateHz,
                            float depth,
                            float panAmount,
                            float sideAmount,
                            float crossfeed)
    {
        if (channelCount < 2 || sampleRate <= 0.0)
            return;

        auto* wetLeft = wetBuffer.getWritePointer(0);
        auto* wetRight = wetBuffer.getWritePointer(1);
        const auto sampleCount = wetBuffer.getNumSamples();
        const auto phaseIncrement = (twoPi * juce::jlimit(0.01f, 8.0f, rateHz)) / static_cast<float>(sampleRate);
        auto localPhase = phase;

        for (int sample = 0; sample < sampleCount; ++sample)
        {
            const auto lfo = std::sin(localPhase);
            const auto quadrature = std::sin(localPhase + (0.5f * juce::MathConstants<float>::pi));
            const auto dryLeft = wetLeft[sample];
            const auto dryRight = wetRight[sample];
            const auto mid = 0.5f * (dryLeft + dryRight);
            const auto side = 0.5f * (dryLeft - dryRight);
            const auto movingPan = lfo * panAmount * depth;
            const auto movingSide = quadrature * sideAmount * depth;
            const auto pushedLeft = mid + (side * (1.0f + movingSide)) + (mid * movingPan) - (dryRight * crossfeed);
            const auto pushedRight = mid - (side * (1.0f + movingSide)) - (mid * movingPan) - (dryLeft * crossfeed);
            wetLeft[sample] = pushedLeft;
            wetRight[sample] = pushedRight;
            localPhase = wrapPhase(localPhase + phaseIncrement);
        }

        phase = localPhase;
    }

    float readInterpolatedSample(const std::vector<float>& buffer, int writePosition, float delaySamples)
    {
        if (buffer.empty())
            return 0.0f;

        const auto bufferSize = static_cast<int>(buffer.size());
        auto readPosition = static_cast<float>(writePosition) - delaySamples;
        while (readPosition < 0.0f)
            readPosition += static_cast<float>(bufferSize);

        const auto indexA = static_cast<int>(readPosition) % bufferSize;
        const auto indexB = (indexA + 1) % bufferSize;
        const auto fraction = readPosition - static_cast<float>(indexA);
        return buffer[static_cast<size_t>(indexA)] + ((buffer[static_cast<size_t>(indexB)] - buffer[static_cast<size_t>(indexA)]) * fraction);
    }

    float processAllPassSample(float input, float coefficient, std::array<float, 2>& state)
    {
        const auto output = (-coefficient * input) + state[0] + (coefficient * state[1]);
        state[0] = input;
        state[1] = output;
        return output;
    }

    float calculateSoftKneeGainReductionDb(float inputDb, float thresholdDb, float ratio, float kneeDb)
    {
        const auto overshoot = inputDb - thresholdDb;
        if (kneeDb > 0.0f)
        {
            const auto halfKnee = 0.5f * kneeDb;
            if (overshoot <= -halfKnee)
                return 0.0f;

            if (overshoot < halfKnee)
            {
                const auto x = overshoot + halfKnee;
                return (1.0f - (1.0f / ratio)) * (x * x) / (2.0f * kneeDb);
            }
        }

        if (overshoot <= 0.0f)
            return 0.0f;

        return (1.0f - (1.0f / ratio)) * overshoot;
    }

    void blendModulationToStereo(juce::AudioBuffer<float>& destination,
                                 const juce::AudioBuffer<float>& wetBuffer,
                                 int channelCount,
                                 float wetAmount,
                                 float stereoWidth)
    {
        const auto sampleCount = destination.getNumSamples();
        const auto clampedWet = juce::jlimit(0.0f, 1.0f, wetAmount);
        const auto clampedWidth = juce::jlimit(0.0f, 1.35f, stereoWidth);

        if (channelCount < 2)
        {
            for (int sample = 0; sample < sampleCount; ++sample)
            {
                const auto dry = destination.getSample(0, sample);
                const auto wet = wetBuffer.getSample(0, sample);
                destination.setSample(0, sample, dry + ((wet - dry) * clampedWet));
            }
            return;
        }

        auto* left = destination.getWritePointer(0);
        auto* right = destination.getWritePointer(1);
        const auto* wetLeft = wetBuffer.getReadPointer(0);
        const auto* wetRight = wetBuffer.getReadPointer(1);
        const auto sideScale = 1.0f + (clampedWidth * 0.85f);
        const auto crossfeed = clampedWidth * 0.12f;

        for (int sample = 0; sample < sampleCount; ++sample)
        {
            const auto dryLeft = left[sample];
            const auto dryRight = right[sample];
            const auto wetMid = 0.5f * (wetLeft[sample] + wetRight[sample]);
            const auto wetSide = 0.5f * (wetLeft[sample] - wetRight[sample]);
            const auto widenedLeft = wetMid + (wetSide * sideScale) - (wetRight[sample] * crossfeed);
            const auto widenedRight = wetMid - (wetSide * sideScale) - (wetLeft[sample] * crossfeed);

            left[sample] = dryLeft + ((widenedLeft - dryLeft) * clampedWet);
            right[sample] = dryRight + ((widenedRight - dryRight) * clampedWet);
        }
    }

    struct EffectPreset
    {
        const char* name;
        std::array<std::pair<const char*, float>, 5> values;
    };

    const auto& getEffectPresetDefinitions(VayuAudioProcessor::EffectSlot effectSlot)
    {
        static const std::array<EffectPreset, 4> distortionPresets{{
            { "Warm Tape", {{{ distortionEnabledId, 1.0f }, { distortionModeId, 0.0f }, { distortionDriveId, 10.0f }, { distortionToneId, 0.48f }, { distortionMixId, 0.38f }}} },
            { "Broken Amp", {{{ distortionEnabledId, 1.0f }, { distortionModeId, 1.0f }, { distortionDriveId, 18.0f }, { distortionToneId, 0.66f }, { distortionMixId, 0.58f }}} },
            { "Crushed Radio", {{{ distortionEnabledId, 1.0f }, { distortionModeId, 2.0f }, { distortionDriveId, 24.0f }, { distortionToneId, 0.24f }, { distortionMixId, 0.72f }}} },
            { "Bypass", {{{ distortionEnabledId, 0.0f }, { distortionModeId, 0.0f }, { distortionDriveId, 6.0f }, { distortionToneId, 0.65f }, { distortionMixId, 0.35f }}} }
        }};
        static const std::array<EffectPreset, 4> chorusPresets{{
            { "Dimension D", {{{ chorusEnabledId, 1.0f }, { chorusModeId, 0.0f }, { chorusRateId, 0.10f }, { chorusDepthId, 0.24f }, { chorusMixId, 0.28f }}} },
            { "Studio Ensemble", {{{ chorusEnabledId, 1.0f }, { chorusModeId, 1.0f }, { chorusRateId, 0.26f }, { chorusDepthId, 0.50f }, { chorusMixId, 0.40f }}} },
            { "MicroPitch Luxe", {{{ chorusEnabledId, 1.0f }, { chorusModeId, 2.0f }, { chorusRateId, 0.18f }, { chorusDepthId, 0.64f }, { chorusMixId, 0.54f }}} },
            { "Bypass", {{{ chorusEnabledId, 0.0f }, { chorusModeId, 0.0f }, { chorusRateId, 0.8f }, { chorusDepthId, 0.45f }, { chorusMixId, 0.30f }}} }
        }};
        static const std::array<EffectPreset, 4> flangerPresets{{
            { "Tape Jet", {{{ flangerEnabledId, 1.0f }, { flangerModeId, 0.0f }, { flangerRateId, 0.08f }, { flangerDepthId, 0.46f }, { flangerMixId, 0.22f }}} },
            { "Resonant Sweep", {{{ flangerEnabledId, 1.0f }, { flangerModeId, 1.0f }, { flangerRateId, 0.16f }, { flangerDepthId, 0.76f }, { flangerMixId, 0.36f }}} },
            { "Through-Zero Jet", {{{ flangerEnabledId, 1.0f }, { flangerModeId, 2.0f }, { flangerRateId, 0.10f }, { flangerDepthId, 0.94f }, { flangerMixId, 0.48f }}} },
            { "Bypass", {{{ flangerEnabledId, 0.0f }, { flangerModeId, 0.0f }, { flangerRateId, 0.35f }, { flangerDepthId, 0.55f }, { flangerMixId, 0.25f }}} }
        }};
        static const std::array<EffectPreset, 4> phaserPresets{{
            { "Script Sweep", {{{ phaserEnabledId, 1.0f }, { phaserModeId, 0.0f }, { phaserRateId, 0.12f }, { phaserDepthId, 0.36f }, { phaserMixId, 0.24f }}} },
            { "4-Stage Throb", {{{ phaserEnabledId, 1.0f }, { phaserModeId, 1.0f }, { phaserRateId, 0.30f }, { phaserDepthId, 0.68f }, { phaserMixId, 0.34f }}} },
            { "Vibe Notches", {{{ phaserEnabledId, 1.0f }, { phaserModeId, 2.0f }, { phaserRateId, 0.20f }, { phaserDepthId, 0.82f }, { phaserMixId, 0.42f }}} },
            { "Bypass", {{{ phaserEnabledId, 0.0f }, { phaserModeId, 0.0f }, { phaserRateId, 0.4f }, { phaserDepthId, 0.55f }, { phaserMixId, 0.30f }}} }
        }};
        static const std::array<EffectPreset, 4> delayPresets{{
            { "Digital Echo", {{{ delayEnabledId, 1.0f }, { delayModeId, 0.0f }, { delayTimeId, 0.24f }, { delayFeedbackId, 0.34f }, { delayMixId, 0.24f }}} },
            { "Tape Wobble", {{{ delayEnabledId, 1.0f }, { delayModeId, 1.0f }, { delayTimeId, 0.42f }, { delayFeedbackId, 0.52f }, { delayMixId, 0.34f }}} },
            { "Ping Pong Wide", {{{ delayEnabledId, 1.0f }, { delayModeId, 2.0f }, { delayTimeId, 0.28f }, { delayFeedbackId, 0.72f }, { delayMixId, 0.62f }}} },
            { "Bypass", {{{ delayEnabledId, 0.0f }, { delayModeId, 0.0f }, { delayTimeId, 0.32f }, { delayFeedbackId, 0.35f }, { delayMixId, 0.25f }}} }
        }};
        static const std::array<EffectPreset, 4> stereoPresets{{
            { "Cinema Wide", {{{ stereoEnabledId, 1.0f }, { stereoModeId, 0.0f }, { stereoRateId, 0.18f }, { stereoDepthId, 0.54f }, { stereoMixId, 0.42f }}} },
            { "Headphone Motion", {{{ stereoEnabledId, 1.0f }, { stereoModeId, 1.0f }, { stereoRateId, 0.42f }, { stereoDepthId, 0.62f }, { stereoMixId, 0.52f }}} },
            { "Orbit Field", {{{ stereoEnabledId, 1.0f }, { stereoModeId, 2.0f }, { stereoRateId, 0.74f }, { stereoDepthId, 0.84f }, { stereoMixId, 0.64f }}} },
            { "Bypass", {{{ stereoEnabledId, 0.0f }, { stereoModeId, 0.0f }, { stereoRateId, 0.45f }, { stereoDepthId, 0.55f }, { stereoMixId, 0.40f }}} }
        }};
        static const std::array<EffectPreset, 4> reverbPresets{{
            { "Tight Room", {{{ reverbEnabledId, 1.0f }, { reverbModeId, 0.0f }, { reverbSizeId, 0.28f }, { reverbDampingId, 0.48f }, { reverbMixId, 0.18f }}} },
            { "Sky Hall", {{{ reverbEnabledId, 1.0f }, { reverbModeId, 1.0f }, { reverbSizeId, 0.78f }, { reverbDampingId, 0.24f }, { reverbMixId, 0.38f }}} },
            { "Glass Plate", {{{ reverbEnabledId, 1.0f }, { reverbModeId, 2.0f }, { reverbSizeId, 0.52f }, { reverbDampingId, 0.12f }, { reverbMixId, 0.44f }}} },
            { "Bypass", {{{ reverbEnabledId, 0.0f }, { reverbModeId, 0.0f }, { reverbSizeId, 0.45f }, { reverbDampingId, 0.35f }, { reverbMixId, 0.22f }}} }
        }};
        static const std::array<EffectPreset, 4> compressorPresets{{
            { "Bus Glue", {{{ compressorEnabledId, 1.0f }, { compressorModeId, 1.0f }, { compressorThresholdId, -18.0f }, { compressorAmountId, 0.42f }, { compressorMixId, 1.0f }}} },
            { "Forward Punch", {{{ compressorEnabledId, 1.0f }, { compressorModeId, 2.0f }, { compressorThresholdId, -14.0f }, { compressorAmountId, 0.62f }, { compressorMixId, 0.88f }}} },
            { "Sidechain Pump", {{{ compressorEnabledId, 1.0f }, { compressorModeId, 3.0f }, { compressorThresholdId, -26.0f }, { compressorAmountId, 0.78f }, { compressorMixId, 0.74f }}} },
            { "Bypass", {{{ compressorEnabledId, 0.0f }, { compressorModeId, 0.0f }, { compressorThresholdId, -18.0f }, { compressorAmountId, 0.40f }, { compressorMixId, 1.0f }}} }
        }};

        switch (effectSlot)
        {
            case VayuAudioProcessor::EffectSlot::distortion: return distortionPresets;
            case VayuAudioProcessor::EffectSlot::chorus: return chorusPresets;
            case VayuAudioProcessor::EffectSlot::flanger: return flangerPresets;
            case VayuAudioProcessor::EffectSlot::phaser: return phaserPresets;
            case VayuAudioProcessor::EffectSlot::delay: return delayPresets;
            case VayuAudioProcessor::EffectSlot::stereo: return stereoPresets;
            case VayuAudioProcessor::EffectSlot::reverb: return reverbPresets;
            case VayuAudioProcessor::EffectSlot::compressor: return compressorPresets;
        }
        return distortionPresets;
    }

    std::array<const char*, 5> getEffectParameterIds(VayuAudioProcessor::EffectSlot effectSlot)
    {
        switch (effectSlot)
        {
            case VayuAudioProcessor::EffectSlot::distortion: return { distortionEnabledId, distortionModeId, distortionDriveId, distortionToneId, distortionMixId };
            case VayuAudioProcessor::EffectSlot::chorus: return { chorusEnabledId, chorusModeId, chorusRateId, chorusDepthId, chorusMixId };
            case VayuAudioProcessor::EffectSlot::flanger: return { flangerEnabledId, flangerModeId, flangerRateId, flangerDepthId, flangerMixId };
            case VayuAudioProcessor::EffectSlot::phaser: return { phaserEnabledId, phaserModeId, phaserRateId, phaserDepthId, phaserMixId };
            case VayuAudioProcessor::EffectSlot::delay: return { delayEnabledId, delayModeId, delayTimeId, delayFeedbackId, delayMixId };
            case VayuAudioProcessor::EffectSlot::stereo: return { stereoEnabledId, stereoModeId, stereoRateId, stereoDepthId, stereoMixId };
            case VayuAudioProcessor::EffectSlot::reverb: return { reverbEnabledId, reverbModeId, reverbSizeId, reverbDampingId, reverbMixId };
            case VayuAudioProcessor::EffectSlot::compressor: return { compressorEnabledId, compressorModeId, compressorThresholdId, compressorAmountId, compressorMixId };
        }
        return { distortionEnabledId, distortionModeId, distortionDriveId, distortionToneId, distortionMixId };
    }

    juce::String getEffectFolderName(VayuAudioProcessor::EffectSlot effectSlot)
    {
        switch (effectSlot)
        {
            case VayuAudioProcessor::EffectSlot::distortion: return "Distortion";
            case VayuAudioProcessor::EffectSlot::chorus: return "Chorus";
            case VayuAudioProcessor::EffectSlot::flanger: return "Flanger";
            case VayuAudioProcessor::EffectSlot::phaser: return "Phaser";
            case VayuAudioProcessor::EffectSlot::delay: return "Delay";
            case VayuAudioProcessor::EffectSlot::stereo: return "Stereo";
            case VayuAudioProcessor::EffectSlot::reverb: return "Reverb";
            case VayuAudioProcessor::EffectSlot::compressor: return "Compressor";
        }
        return "Effect";
    }

    Coefficients::Ptr makeLowShelf(double sampleRate, float frequency, float q, float gainDb)
    {
        return Coefficients::makeLowShelf(sampleRate, frequency, q, juce::Decibels::decibelsToGain(gainDb));
    }

    Coefficients::Ptr makePeak(double sampleRate, float frequency, float q, float gainDb)
    {
        return Coefficients::makePeakFilter(sampleRate, frequency, q, juce::Decibels::decibelsToGain(gainDb));
    }

    Coefficients::Ptr makeHighShelf(double sampleRate, float frequency, float q, float gainDb)
    {
        return Coefficients::makeHighShelf(sampleRate, frequency, q, juce::Decibels::decibelsToGain(gainDb));
    }

    juce::ParameterID makeParameterId(const char* id, int versionHint)
    {
        return { id, versionHint };
    }

    float clampFilterFrequency(double sampleRate, float frequency)
    {
        const auto nyquistSafeLimit = static_cast<float>(juce::jmax(40.0, sampleRate * 0.48));
        return juce::jlimit(20.0f, nyquistSafeLimit, frequency);
    }

    constexpr std::array<const char*, 56> parameterIds{{
        lowShelfFreqId, lowShelfGainId, lowShelfQId,
        lowMidFreqId, lowMidGainId, lowMidQId,
        midFreqId, midGainId, midQId,
        highMidFreqId, highMidGainId, highMidQId,
        highShelfFreqId, highShelfGainId, highShelfQId,
        outputGainId,
        distortionEnabledId, distortionModeId, distortionDriveId, distortionToneId, distortionMixId,
        chorusEnabledId, chorusModeId, chorusRateId, chorusDepthId, chorusMixId,
        flangerEnabledId, flangerModeId, flangerRateId, flangerDepthId, flangerMixId,
        phaserEnabledId, phaserModeId, phaserRateId, phaserDepthId, phaserMixId,
        delayEnabledId, delayModeId, delayTimeId, delayFeedbackId, delayMixId,
        stereoEnabledId, stereoModeId, stereoRateId, stereoDepthId, stereoMixId,
        reverbEnabledId, reverbModeId, reverbSizeId, reverbDampingId, reverbMixId,
        compressorEnabledId, compressorModeId, compressorThresholdId, compressorAmountId, compressorMixId
    }};

    std::array<int, effectSlotCount> makeDefaultEffectOrder()
    {
        return { 0, 7, 1, 2, 3, 4, 5, 6 };
    }

    juce::String getEffectOrderPropertyName(int index)
    {
        return effectOrderPropertyPrefix + juce::String(index);
    }

    std::array<int, effectSlotCount> sanitiseEffectOrder(const std::array<int, effectSlotCount>& candidate)
    {
        std::array<int, effectSlotCount> result {};
        std::array<bool, effectSlotCount> seen {};
        int writeIndex = 0;

        for (int value : candidate)
        {
            if (juce::isPositiveAndBelow(value, effectSlotCount) && !seen[static_cast<size_t>(value)])
            {
                result[static_cast<size_t>(writeIndex++)] = value;
                seen[static_cast<size_t>(value)] = true;
            }
        }

        for (int value = 0; value < effectSlotCount; ++value)
        {
            if (!seen[static_cast<size_t>(value)])
                result[static_cast<size_t>(writeIndex++)] = value;
        }

        return result;
    }

    std::array<int, effectSlotCount> readEffectOrderFromState(const juce::ValueTree& state)
    {
        auto order = makeDefaultEffectOrder();
        for (int i = 0; i < effectSlotCount; ++i)
            order[static_cast<size_t>(i)] = static_cast<int>(state.getProperty(getEffectOrderPropertyName(i), order[static_cast<size_t>(i)]));
        return sanitiseEffectOrder(order);
    }

    void writeEffectOrderToState(juce::ValueTree& state, const std::array<int, effectSlotCount>& order)
    {
        const auto sanitisedOrder = sanitiseEffectOrder(order);
        for (int i = 0; i < effectSlotCount; ++i)
            state.setProperty(getEffectOrderPropertyName(i), sanitisedOrder[static_cast<size_t>(i)], nullptr);
    }

#if JUCE_MAC && JucePlugin_Build_Standalone
    void clearStandaloneSavedState()
    {
        auto savedStateDirectory = juce::File::getSpecialLocation(juce::File::userHomeDirectory)
                                       .getChildFile("Library")
                                       .getChildFile("Saved Application State")
                                       .getChildFile("com.supash.vayu.savedState");
        if (savedStateDirectory.exists())
            savedStateDirectory.deleteRecursively();
    }
#endif
}

VayuAudioProcessor::VayuAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                      .withInput("Sidechain", juce::AudioChannelSet::stereo(), false)
                                      .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, &undoManager, stateId, createParameterLayout())
{
    setEffectOrder(makeDefaultEffectOrder());

#if JUCE_MAC && JucePlugin_Build_Standalone
    clearStandaloneSavedState();
#endif

    for (const auto* parameterId : parameterIds)
        parameters.addParameterListener(parameterId, this);

    applyPreset(0);
}

VayuAudioProcessor::~VayuAudioProcessor()
{
    for (const auto* parameterId : parameterIds)
        parameters.removeParameterListener(parameterId, this);
}

// Thread-safe getter/setter for effect order
void VayuAudioProcessor::setEffectOrder(const std::array<int, effectCount>& newOrder)
{
    const auto sanitisedOrder = sanitiseEffectOrder(newOrder);
    for (int i = 0; i < static_cast<int>(effectCount); ++i)
        effectOrder[static_cast<size_t>(i)] = sanitisedOrder[static_cast<size_t>(i)];

    writeEffectOrderToState(parameters.state, sanitisedOrder);
}

std::array<int, VayuAudioProcessor::effectCount> VayuAudioProcessor::getEffectOrder() const
{
    std::array<int, effectCount> result;
    for (int i = 0; i < static_cast<int>(effectCount); ++i)
        result[static_cast<size_t>(i)] = effectOrder[static_cast<size_t>(i)].load();
    return result;
}

juce::AudioProcessorValueTreeState::ParameterLayout VayuAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> layout;

    layout.push_back(std::make_unique<Parameter>(makeParameterId(lowShelfFreqId, 1), "Low Shelf Frequency", makeFrequencyRange(30.0f, 400.0f, 0.4f), 110.0f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(lowShelfGainId, 2), "Low Shelf Gain", juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(lowShelfQId, 3), "Low Shelf Q", juce::NormalisableRange<float>(0.4f, 1.4f, 0.01f), 0.75f));

    layout.push_back(std::make_unique<Parameter>(makeParameterId(lowMidFreqId, 4), "Low Mid Frequency", makeFrequencyRange(120.0f, 1200.0f, 0.4f), 260.0f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(lowMidGainId, 5), "Low Mid Gain", juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(lowMidQId, 6), "Low Mid Q", juce::NormalisableRange<float>(0.3f, 4.0f, 0.01f), 0.90f));

    layout.push_back(std::make_unique<Parameter>(makeParameterId(midFreqId, 7), "Mid Frequency", makeFrequencyRange(400.0f, 4000.0f, 0.35f), 1000.0f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(midGainId, 8), "Mid Gain", juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(midQId, 9), "Mid Q", juce::NormalisableRange<float>(0.3f, 4.0f, 0.01f), 1.00f));

    layout.push_back(std::make_unique<Parameter>(makeParameterId(highMidFreqId, 10), "High Mid Frequency", makeFrequencyRange(1200.0f, 9000.0f, 0.35f), 3600.0f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(highMidGainId, 11), "High Mid Gain", juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(highMidQId, 12), "High Mid Q", juce::NormalisableRange<float>(0.3f, 4.0f, 0.01f), 0.90f));

    layout.push_back(std::make_unique<Parameter>(makeParameterId(highShelfFreqId, 13), "High Shelf Frequency", makeFrequencyRange(3000.0f, 18000.0f, 0.3f), 9200.0f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(highShelfGainId, 14), "High Shelf Gain", juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(highShelfQId, 15), "High Shelf Q", juce::NormalisableRange<float>(0.4f, 1.4f, 0.01f), 0.75f));

    layout.push_back(std::make_unique<Parameter>(makeParameterId(outputGainId, 16), "Output Gain", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));

    layout.push_back(std::make_unique<juce::AudioParameterBool>(makeParameterId(distortionEnabledId, 35), "Distortion Enabled", false));
    layout.push_back(std::make_unique<juce::AudioParameterChoice>(makeParameterId(distortionModeId, 36), "Distortion Mode", juce::StringArray { "Warm", "Crunch", "Clip" }, 0));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(distortionDriveId, 17), "Distortion Drive", juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 6.0f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(distortionToneId, 18), "Distortion Tone", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.65f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(distortionMixId, 19), "Distortion Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.35f));

    layout.push_back(std::make_unique<juce::AudioParameterBool>(makeParameterId(chorusEnabledId, 37), "Chorus Enabled", false));
    layout.push_back(std::make_unique<juce::AudioParameterChoice>(makeParameterId(chorusModeId, 38), "Chorus Mode", juce::StringArray { "Clean", "Wide", "Ensemble" }, 0));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(chorusRateId, 20), "Chorus Rate", juce::NormalisableRange<float>(0.05f, 5.0f, 0.01f), 0.8f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(chorusDepthId, 21), "Chorus Depth", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.45f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(chorusMixId, 22), "Chorus Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));

    layout.push_back(std::make_unique<juce::AudioParameterBool>(makeParameterId(flangerEnabledId, 39), "Flanger Enabled", false));
    layout.push_back(std::make_unique<juce::AudioParameterChoice>(makeParameterId(flangerModeId, 40), "Flanger Mode", juce::StringArray { "Jet", "Liquid", "Metal" }, 0));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(flangerRateId, 23), "Flanger Rate", juce::NormalisableRange<float>(0.05f, 3.5f, 0.01f), 0.35f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(flangerDepthId, 24), "Flanger Depth", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.55f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(flangerMixId, 25), "Flanger Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.25f));

    layout.push_back(std::make_unique<juce::AudioParameterBool>(makeParameterId(phaserEnabledId, 41), "Phaser Enabled", false));
    layout.push_back(std::make_unique<juce::AudioParameterChoice>(makeParameterId(phaserModeId, 42), "Phaser Mode", juce::StringArray { "Gentle", "Deep", "Vocal" }, 0));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(phaserRateId, 26), "Phaser Rate", juce::NormalisableRange<float>(0.05f, 4.0f, 0.01f), 0.4f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(phaserDepthId, 27), "Phaser Depth", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.55f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(phaserMixId, 28), "Phaser Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));

    layout.push_back(std::make_unique<juce::AudioParameterBool>(makeParameterId(delayEnabledId, 43), "Delay Enabled", false));
    layout.push_back(std::make_unique<juce::AudioParameterChoice>(makeParameterId(delayModeId, 44), "Delay Mode", juce::StringArray { "Digital", "Tape", "Ping Pong" }, 0));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(delayTimeId, 29), "Delay Time", juce::NormalisableRange<float>(0.05f, 1.2f, 0.001f), 0.32f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(delayFeedbackId, 30), "Delay Feedback", juce::NormalisableRange<float>(0.0f, 0.92f, 0.01f), 0.35f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(delayMixId, 31), "Delay Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.25f));

    layout.push_back(std::make_unique<juce::AudioParameterBool>(makeParameterId(stereoEnabledId, 47), "Stereo Enabled", false));
    layout.push_back(std::make_unique<juce::AudioParameterChoice>(makeParameterId(stereoModeId, 48), "Stereo Mode", juce::StringArray { "Wide", "Auto Pan", "Orbit" }, 0));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(stereoRateId, 49), "Stereo Rate", juce::NormalisableRange<float>(0.02f, 6.0f, 0.01f), 0.45f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(stereoDepthId, 50), "Stereo Width", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.55f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(stereoMixId, 51), "Stereo Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.40f));

    layout.push_back(std::make_unique<juce::AudioParameterBool>(makeParameterId(reverbEnabledId, 45), "Reverb Enabled", false));
    layout.push_back(std::make_unique<juce::AudioParameterChoice>(makeParameterId(reverbModeId, 46), "Reverb Mode", juce::StringArray { "Room", "Hall", "Plate" }, 0));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(reverbSizeId, 32), "Reverb Size", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.45f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(reverbDampingId, 33), "Reverb Damping", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.35f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(reverbMixId, 34), "Reverb Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.22f));

    layout.push_back(std::make_unique<juce::AudioParameterBool>(makeParameterId(compressorEnabledId, 52), "Compressor Enabled", false));
    layout.push_back(std::make_unique<juce::AudioParameterChoice>(makeParameterId(compressorModeId, 53), "Compressor Mode", juce::StringArray { "Clean", "Glue", "Punch", "Pump" }, 0));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(compressorThresholdId, 54), "Compressor Threshold", juce::NormalisableRange<float>(-36.0f, 0.0f, 0.1f), -18.0f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(compressorAmountId, 55), "Compressor Amount", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.40f));
    layout.push_back(std::make_unique<Parameter>(makeParameterId(compressorMixId, 56), "Compressor Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    return { layout.begin(), layout.end() };
}

void VayuAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    processingStateNeedsUpdate.store(true);
    inputLevel.store(0.0f);
    outputLevel.store(0.0f);
    analyserFifoIndex = 0;
    analyserFifo.fill(0.0f);
    analyserData.fill(0.0f);
    spectrumData.fill(0.0f);
    delayFilterState.fill(0.0f);
    distortionToneState.fill(0.0f);
    maxDelaySamples = juce::jmax(1, static_cast<int>(std::ceil(sampleRate * 2.5)));
    delayWritePosition = 0;

    juce::dsp::ProcessSpec spec{};
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 1;

    for (auto& chain : filterChains)
    {
        chain.prepare(spec);
        chain.get<5>().setRampDurationSeconds(0.02);
        chain.reset();
    }

    for (auto& flanger : flangerProcessors)
    {
        flanger.prepare(spec);
        flanger.reset();
    }

    const auto chorusBufferLength = juce::jmax(1, static_cast<int>(std::ceil(sampleRate * 0.08)));
    chorusWritePosition = 0;
    chorusPhase = 0.0f;
    phaserPhase = 0.0f;
    stereoPhase = 0.0f;
    compressorEnvelope = 0.0f;
    compressorRmsState = 0.0f;
    compressorGainState = 1.0f;
    compressorSidechainLowpassState = 0.0f;
    compressorGainReductionDb.store(0.0f);
    phaserFeedbackState.fill(0.0f);
    for (auto& channelState : phaserStageState)
        for (auto& stageState : channelState)
            stageState = { 0.0f, 0.0f };

    for (auto& chorusDelayBuffer : chorusDelayBuffers)
        chorusDelayBuffer.assign(static_cast<size_t>(chorusBufferLength), 0.0f);

    for (auto& delayBuffer : delayBuffers)
        delayBuffer.assign(static_cast<size_t>(maxDelaySamples), 0.0f);

    modulationScratchBuffer.setSize(static_cast<int>(stereoChannels), samplesPerBlock, false, false, true);
    modulationScratchBuffer.clear();
    reverbProcessor.reset();
    updateProcessingState();
}

void VayuAudioProcessor::releaseResources()
{
}

bool VayuAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto mainInput = layouts.getMainInputChannelSet();
    const auto mainOutput = layouts.getMainOutputChannelSet();
    const auto sidechain = layouts.inputBuses.size() > 1 ? layouts.getChannelSet(true, 1) : juce::AudioChannelSet::disabled();

    if (mainInput.isDisabled() || mainOutput.isDisabled())
        return false;

    const auto inputIsSupported = mainInput == juce::AudioChannelSet::mono()
                                  || mainInput == juce::AudioChannelSet::stereo();

    const auto outputIsSupported = mainOutput == juce::AudioChannelSet::mono()
                                   || mainOutput == juce::AudioChannelSet::stereo();

    const auto sidechainSupported = sidechain.isDisabled()
                                    || sidechain == juce::AudioChannelSet::mono()
                                    || sidechain == juce::AudioChannelSet::stereo();

    return inputIsSupported && outputIsSupported && sidechainSupported && mainInput == mainOutput;
}

// Master effect processor that respects the modular order
void VayuAudioProcessor::processEffectsInOrder(juce::AudioBuffer<float>& buffer,
                                                  int channelCount,
                                                  const juce::AudioBuffer<float>* sidechainBuffer)
{
    std::array<int, effectCount> currentOrder;
    for (int i = 0; i < static_cast<int>(effectCount); ++i)
        currentOrder[static_cast<size_t>(i)] = effectOrder[static_cast<size_t>(i)].load();

    for (int i = 0; i < static_cast<int>(effectCount); ++i)
    {
        switch (currentOrder[static_cast<size_t>(i)])
        {
            case 0:
                if (getParameterValue(parameters, distortionEnabledId) > 0.5f)
                    processDistortion(buffer, channelCount);
                break;
            case 1:
                if (getParameterValue(parameters, chorusEnabledId) > 0.5f)
                    processChorus(buffer, channelCount);
                break;
            case 2:
                if (getParameterValue(parameters, flangerEnabledId) > 0.5f)
                    processFlanger(buffer, channelCount);
                break;
            case 3:
                if (getParameterValue(parameters, phaserEnabledId) > 0.5f)
                    processPhaser(buffer, channelCount);
                break;
            case 4:
                if (getParameterValue(parameters, delayEnabledId) > 0.5f)
                    processDelay(buffer, channelCount);
                break;
            case 5:
                if (getParameterValue(parameters, stereoEnabledId) > 0.5f)
                    processStereo(buffer, channelCount);
                break;
            case 6:
                if (getParameterValue(parameters, reverbEnabledId) > 0.5f)
                    processReverb(buffer, channelCount);
                break;
            case 7:
                if (getParameterValue(parameters, compressorEnabledId) > 0.5f)
                    processCompressor(buffer, channelCount, sidechainBuffer);
                break;
            default:
                break;
        }
    }
}

void VayuAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const auto totalOutputChannels = getTotalNumOutputChannels();
    const auto mainInputChannels = getMainBusNumInputChannels();
    const auto hasSidechain = getBusCount(true) > 1 && getBus(true, 1) != nullptr && getBus(true, 1)->isEnabled();

    for (auto channel = mainInputChannels; channel < totalOutputChannels; ++channel)
        buffer.clear(channel, 0, buffer.getNumSamples());

    if (mainInputChannels == 1 && totalOutputChannels > 1 && buffer.getNumChannels() > 1)
    {
        for (auto channel = 1; channel < juce::jmin(totalOutputChannels, buffer.getNumChannels()); ++channel)
            buffer.copyFrom(channel, 0, buffer, 0, 0, buffer.getNumSamples());
    }

    const auto channelsToProcess = juce::jmin(static_cast<int>(filterChains.size()),
                                              juce::jmin(totalOutputChannels, buffer.getNumChannels()));
    if (channelsToProcess <= 0)
    {
        inputLevel.store(0.0f);
        outputLevel.store(0.0f);
        return;
    }

    inputLevel.store(measurePeakLevel(buffer, channelsToProcess));

    if (currentSampleRate != getSampleRate() && getSampleRate() > 0.0)
    {
        currentSampleRate = getSampleRate();
        processingStateNeedsUpdate.store(true);
    }

    updateProcessingState();

    if (getParameterValue(parameters, compressorEnabledId) <= 0.5f)
        compressorGainReductionDb.store(0.0f);

    // First, apply EQ (always first in chain - this is fixed)
    juce::dsp::AudioBlock<float> block(buffer);
    for (auto channel = 0; channel < channelsToProcess; ++channel)
    {
        auto monoBlock = block.getSingleChannelBlock(static_cast<size_t>(channel));
        juce::dsp::ProcessContextReplacing<float> context(monoBlock);
        filterChains[static_cast<size_t>(channel)].process(context);
    }

    // Then process effects in the user-defined modular order
    const juce::AudioBuffer<float>* sidechainBuffer = nullptr;
    juce::AudioBuffer<float> externalSidechain;
    if (hasSidechain)
    {
        externalSidechain.setSize(juce::jmax(1, getBus(true, 1)->getNumberOfChannels()),
                                  buffer.getNumSamples(),
                                  false, false, true);
        externalSidechain.makeCopyOf(getBusBuffer(buffer, true, 1), true);
        sidechainBuffer = &externalSidechain;
    }

    processEffectsInOrder(buffer, channelsToProcess, sidechainBuffer);

    outputLevel.store(measurePeakLevel(buffer, channelsToProcess));

    if (channelsToProcess > 0)
    {
        const auto* analyserSamples = buffer.getReadPointer(0);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            pushNextAnalyserSample(analyserSamples[sample]);
    }
}

juce::AudioProcessorEditor* VayuAudioProcessor::createEditor()
{
    return new VayuAudioProcessorEditor(*this);
}

bool VayuAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String VayuAudioProcessor::getName() const
{
    return "Vayu";
}

bool VayuAudioProcessor::acceptsMidi() const
{
    return false;
}

bool VayuAudioProcessor::producesMidi() const
{
    return false;
}

bool VayuAudioProcessor::isMidiEffect() const
{
    return false;
}

double VayuAudioProcessor::getTailLengthSeconds() const
{
    const auto delayEnabled = getParameterValue(parameters, delayEnabledId) > 0.5f;
    const auto reverbEnabled = getParameterValue(parameters, reverbEnabledId) > 0.5f;

    auto tailSeconds = 0.0;
    if (delayEnabled)
        tailSeconds = juce::jmax(tailSeconds, static_cast<double>(getParameterValue(parameters, delayTimeId) * 4.0f));
    if (reverbEnabled)
        tailSeconds = juce::jmax(tailSeconds, 1.5 + (static_cast<double>(getParameterValue(parameters, reverbSizeId)) * 6.0));

    return tailSeconds;
}

int VayuAudioProcessor::getNumPrograms()
{
    return static_cast<int>(getPresetDefinitions().size());
}

int VayuAudioProcessor::getCurrentProgram()
{
    return currentPresetIndex;
}

void VayuAudioProcessor::setCurrentProgram(int index)
{
    applyPreset(index);
}

const juce::String VayuAudioProcessor::getProgramName(int index)
{
    const auto presets = getPresetNames();
    if (juce::isPositiveAndBelow(index, presets.size()))
        return presets[index];
    return {};
}

void VayuAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void VayuAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    state.setProperty(selectedPresetProperty, currentPresetIndex, nullptr);
    state.setProperty(selectedRackPresetProperty, currentRackPresetIndex, nullptr);
    writeEffectOrderToState(state, getEffectOrder());

    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, destData);
}

void VayuAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
    {
        const auto state = juce::ValueTree::fromXml(*xml);
        if (state.isValid())
        {
            parameters.replaceState(state);
            currentPresetIndex = juce::jlimit(0, getNumPrograms() - 1, static_cast<int>(state.getProperty(selectedPresetProperty, 0)));
            currentRackPresetIndex = juce::jlimit(0, static_cast<int>(getRackPresetDefinitions().size()) - 1, static_cast<int>(state.getProperty(selectedRackPresetProperty, 0)));
            setEffectOrder(readEffectOrderFromState(state));
        }
    }
    updateProcessingState();
}

void VayuAudioProcessor::applyPreset(int presetIndex)
{
    const auto& presets = getPresetDefinitions();
    const auto clampedIndex = juce::jlimit(0, static_cast<int>(presets.size()) - 1, presetIndex);
    const auto& preset = presets[static_cast<size_t>(clampedIndex)];

    setParameterValue(parameters, lowShelfFreqId, preset.lowShelfFreq);
    setParameterValue(parameters, lowShelfGainId, preset.lowShelfGain);
    setParameterValue(parameters, lowShelfQId, preset.lowShelfQ);
    setParameterValue(parameters, lowMidFreqId, preset.lowMidFreq);
    setParameterValue(parameters, lowMidGainId, preset.lowMidGain);
    setParameterValue(parameters, lowMidQId, preset.lowMidQ);
    setParameterValue(parameters, midFreqId, preset.midFreq);
    setParameterValue(parameters, midGainId, preset.midGain);
    setParameterValue(parameters, midQId, preset.midQ);
    setParameterValue(parameters, highMidFreqId, preset.highMidFreq);
    setParameterValue(parameters, highMidGainId, preset.highMidGain);
    setParameterValue(parameters, highMidQId, preset.highMidQ);
    setParameterValue(parameters, highShelfFreqId, preset.highShelfFreq);
    setParameterValue(parameters, highShelfGainId, preset.highShelfGain);
    setParameterValue(parameters, highShelfQId, preset.highShelfQ);
    setParameterValue(parameters, outputGainId, preset.outputGain);
    setParameterValue(parameters, distortionEnabledId, 0.0f);
    setParameterValue(parameters, chorusEnabledId, 0.0f);
    setParameterValue(parameters, flangerEnabledId, 0.0f);
    setParameterValue(parameters, phaserEnabledId, 0.0f);
    setParameterValue(parameters, delayEnabledId, 0.0f);
    setParameterValue(parameters, stereoEnabledId, 0.0f);
    setParameterValue(parameters, reverbEnabledId, 0.0f);
    setParameterValue(parameters, compressorEnabledId, 0.0f);

    currentPresetIndex = clampedIndex;
    parameters.state.setProperty(selectedPresetProperty, currentPresetIndex, nullptr);
    updateProcessingState();
}

int VayuAudioProcessor::getCurrentPresetIndex() const noexcept
{
    return currentPresetIndex;
}

juce::StringArray VayuAudioProcessor::getPresetNames() const
{
    return buildPresetNames();
}

juce::StringArray VayuAudioProcessor::getRackPresetNames() const
{
    return buildRackPresetNames();
}

void VayuAudioProcessor::applyRackPreset(int presetIndex)
{
    const auto& presets = getRackPresetDefinitions();
    const auto clampedIndex = juce::jlimit(0, static_cast<int>(presets.size()) - 1, presetIndex);
    const auto& preset = presets[static_cast<size_t>(clampedIndex)];

    setParameterValue(parameters, distortionEnabledId, preset.distortionEnabled);
    setParameterValue(parameters, distortionModeId, preset.distortionMode);
    setParameterValue(parameters, distortionDriveId, preset.distortionDrive);
    setParameterValue(parameters, distortionToneId, preset.distortionTone);
    setParameterValue(parameters, distortionMixId, preset.distortionMix);
    setParameterValue(parameters, chorusEnabledId, preset.chorusEnabled);
    setParameterValue(parameters, chorusModeId, preset.chorusMode);
    setParameterValue(parameters, chorusRateId, preset.chorusRate);
    setParameterValue(parameters, chorusDepthId, preset.chorusDepth);
    setParameterValue(parameters, chorusMixId, preset.chorusMix);
    setParameterValue(parameters, flangerEnabledId, preset.flangerEnabled);
    setParameterValue(parameters, flangerModeId, preset.flangerMode);
    setParameterValue(parameters, flangerRateId, preset.flangerRate);
    setParameterValue(parameters, flangerDepthId, preset.flangerDepth);
    setParameterValue(parameters, flangerMixId, preset.flangerMix);
    setParameterValue(parameters, phaserEnabledId, preset.phaserEnabled);
    setParameterValue(parameters, phaserModeId, preset.phaserMode);
    setParameterValue(parameters, phaserRateId, preset.phaserRate);
    setParameterValue(parameters, phaserDepthId, preset.phaserDepth);
    setParameterValue(parameters, phaserMixId, preset.phaserMix);
    setParameterValue(parameters, delayEnabledId, preset.delayEnabled);
    setParameterValue(parameters, delayModeId, preset.delayMode);
    setParameterValue(parameters, delayTimeId, preset.delayTime);
    setParameterValue(parameters, delayFeedbackId, preset.delayFeedback);
    setParameterValue(parameters, delayMixId, preset.delayMix);
    setParameterValue(parameters, stereoEnabledId, preset.stereoEnabled);
    setParameterValue(parameters, stereoModeId, preset.stereoMode);
    setParameterValue(parameters, stereoRateId, preset.stereoRate);
    setParameterValue(parameters, stereoDepthId, preset.stereoDepth);
    setParameterValue(parameters, stereoMixId, preset.stereoMix);
    setParameterValue(parameters, reverbEnabledId, preset.reverbEnabled);
    setParameterValue(parameters, reverbModeId, preset.reverbMode);
    setParameterValue(parameters, reverbSizeId, preset.reverbSize);
    setParameterValue(parameters, reverbDampingId, preset.reverbDamping);
    setParameterValue(parameters, reverbMixId, preset.reverbMix);
    setParameterValue(parameters, compressorEnabledId, preset.compressorEnabled);
    setParameterValue(parameters, compressorModeId, preset.compressorMode);
    setParameterValue(parameters, compressorThresholdId, preset.compressorThreshold);
    setParameterValue(parameters, compressorAmountId, preset.compressorAmount);
    setParameterValue(parameters, compressorMixId, preset.compressorMix);
    setEffectOrder(preset.effectOrder);

    currentRackPresetIndex = clampedIndex;
    parameters.state.setProperty(selectedRackPresetProperty, currentRackPresetIndex, nullptr);
    updateProcessingState();
}

int VayuAudioProcessor::getCurrentRackPresetIndex() const noexcept
{
    return currentRackPresetIndex;
}

juce::StringArray VayuAudioProcessor::getEffectPresetNames(EffectSlot effectSlot) const
{
    juce::StringArray names;
    for (const auto& preset : getEffectPresetDefinitions(effectSlot))
        names.add(preset.name);
    return names;
}

void VayuAudioProcessor::applyEffectPreset(EffectSlot effectSlot, int presetIndex)
{
    const auto& presets = getEffectPresetDefinitions(effectSlot);
    const auto clampedIndex = juce::jlimit(0, static_cast<int>(presets.size()) - 1, presetIndex);
    for (const auto& [parameterId, value] : presets[static_cast<size_t>(clampedIndex)].values)
        setParameterValue(parameters, parameterId, value);

    currentRackPresetIndex = 0;
    parameters.state.setProperty(selectedRackPresetProperty, currentRackPresetIndex, nullptr);
    updateProcessingState();
}

juce::StringArray VayuAudioProcessor::getUserRackPresetNames() const
{
    juce::StringArray names;
    const auto directory = getUserRackPresetDirectory();
    if (!directory.exists())
        return names;
    for (const auto& file : directory.findChildFiles(juce::File::findFiles, false, "*.vayu-rack"))
        names.add(file.getFileNameWithoutExtension());
    names.sortNatural();
    return names;
}

bool VayuAudioProcessor::saveUserRackPreset(const juce::String& presetName)
{
    const auto trimmedName = presetName.trim();
    if (trimmedName.isEmpty())
        return false;

    auto state = parameters.copyState();
    state.setProperty(selectedPresetProperty, currentPresetIndex, nullptr);
    state.setProperty(selectedRackPresetProperty, currentRackPresetIndex, nullptr);
    state.setProperty("presetName", trimmedName, nullptr);
    writeEffectOrderToState(state, getEffectOrder());

    const auto directory = getUserRackPresetDirectory();
    directory.createDirectory();
    const auto file = directory.getChildFile(trimmedName.replaceCharacter('/', '-').replaceCharacter(':', '-') + ".vayu-rack");

    if (auto xml = state.createXml())
        return xml->writeTo(file);
    return false;
}

bool VayuAudioProcessor::loadUserRackPreset(const juce::String& presetName)
{
    const auto file = getUserRackPresetDirectory().getChildFile(presetName + ".vayu-rack");
    if (!file.existsAsFile())
        return false;

    juce::XmlDocument document(file);
    auto xml = document.getDocumentElement();
    if (xml == nullptr)
        return false;

    const auto state = juce::ValueTree::fromXml(*xml);
    if (!state.isValid())
        return false;

    parameters.replaceState(state);
    currentPresetIndex = juce::jlimit(0, getNumPrograms() - 1, static_cast<int>(state.getProperty(selectedPresetProperty, currentPresetIndex)));
    currentRackPresetIndex = juce::jlimit(0, static_cast<int>(getRackPresetDefinitions().size()) - 1, static_cast<int>(state.getProperty(selectedRackPresetProperty, currentRackPresetIndex)));
    setEffectOrder(readEffectOrderFromState(state));
    updateProcessingState();
    return true;
}

bool VayuAudioProcessor::deleteUserRackPreset(const juce::String& presetName)
{
    const auto trimmedName = presetName.trim();
    if (trimmedName.isEmpty())
        return false;

    const auto file = getUserRackPresetDirectory()
                          .getChildFile(trimmedName.replaceCharacter('/', '-').replaceCharacter(':', '-') + ".vayu-rack");
    if (!file.existsAsFile())
        return false;

    return file.deleteFile();
}

juce::StringArray VayuAudioProcessor::getUserEffectPresetNames(EffectSlot effectSlot) const
{
    juce::StringArray names;
    const auto directory = getUserEffectPresetDirectory(effectSlot);
    if (!directory.exists())
        return names;
    for (const auto& file : directory.findChildFiles(juce::File::findFiles, false, "*.vayu-effect"))
        names.add(file.getFileNameWithoutExtension());
    names.sortNatural();
    return names;
}

bool VayuAudioProcessor::saveUserEffectPreset(EffectSlot effectSlot, const juce::String& presetName)
{
    const auto trimmedName = presetName.trim();
    if (trimmedName.isEmpty())
        return false;

    juce::ValueTree state("EffectPreset");
    state.setProperty("presetName", trimmedName, nullptr);
    state.setProperty("effectSlot", static_cast<int>(effectSlot), nullptr);
    for (const auto* parameterId : getEffectParameterIds(effectSlot))
        state.setProperty(parameterId, getParameterValue(parameters, parameterId), nullptr);

    const auto directory = getUserEffectPresetDirectory(effectSlot);
    directory.createDirectory();
    const auto file = directory.getChildFile(trimmedName.replaceCharacter('/', '-').replaceCharacter(':', '-') + ".vayu-effect");

    if (auto xml = state.createXml())
        return xml->writeTo(file);
    return false;
}

bool VayuAudioProcessor::loadUserEffectPreset(EffectSlot effectSlot, const juce::String& presetName)
{
    const auto file = getUserEffectPresetDirectory(effectSlot).getChildFile(presetName + ".vayu-effect");
    if (!file.existsAsFile())
        return false;

    juce::XmlDocument document(file);
    auto xml = document.getDocumentElement();
    if (xml == nullptr)
        return false;

    const auto state = juce::ValueTree::fromXml(*xml);
    if (!state.isValid())
        return false;

    for (const auto* parameterId : getEffectParameterIds(effectSlot))
        if (state.hasProperty(parameterId))
            setParameterValue(parameters, parameterId, static_cast<float>(state.getProperty(parameterId)));

    currentRackPresetIndex = 0;
    parameters.state.setProperty(selectedRackPresetProperty, currentRackPresetIndex, nullptr);
    updateProcessingState();
    return true;
}

bool VayuAudioProcessor::deleteUserEffectPreset(EffectSlot effectSlot, const juce::String& presetName)
{
    const auto trimmedName = presetName.trim();
    if (trimmedName.isEmpty())
        return false;

    const auto file = getUserEffectPresetDirectory(effectSlot)
                          .getChildFile(trimmedName.replaceCharacter('/', '-').replaceCharacter(':', '-') + ".vayu-effect");
    if (!file.existsAsFile())
        return false;

    return file.deleteFile();
}

float VayuAudioProcessor::getMagnitudeResponseAtFrequency(double frequency) const
{
    const auto sampleRate = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;
    const juce::SpinLock::ScopedLockType lock(responseCurveLock);
    const auto safeFrequency = juce::jlimit(20.0, sampleRate * 0.48, frequency);

    auto magnitude = 1.0;
    for (const auto& coefficients : responseCurveCoefficients)
    {
        if (coefficients != nullptr)
            magnitude *= coefficients->getMagnitudeForFrequency(safeFrequency, sampleRate);
    }
    magnitude *= responseOutputGain;
    return static_cast<float>(magnitude);
}

float VayuAudioProcessor::getInputLevel() const noexcept
{
    return inputLevel.load();
}

float VayuAudioProcessor::getOutputLevel() const noexcept
{
    return outputLevel.load();
}

float VayuAudioProcessor::getCompressorGainReductionDb() const noexcept
{
    return compressorGainReductionDb.load();
}

bool VayuAudioProcessor::canUndo() const noexcept
{
    return undoManager.canUndo();
}

bool VayuAudioProcessor::canRedo() const noexcept
{
    return undoManager.canRedo();
}

void VayuAudioProcessor::undoLastAction()
{
    undoManager.undo();
}

void VayuAudioProcessor::redoLastAction()
{
    undoManager.redo();
}

double VayuAudioProcessor::getCurrentSampleRateValue() const noexcept
{
    return currentSampleRate;
}

void VayuAudioProcessor::copySpectrumData(std::array<float, 128>& destination) const
{
    const juce::SpinLock::ScopedLockType lock(analyserLock);
    destination = spectrumData;
}

float VayuAudioProcessor::getParameterValue(const juce::AudioProcessorValueTreeState& state,
                                               const juce::String& parameterId)
{
    if (auto* value = state.getRawParameterValue(parameterId))
        return value->load();
    jassertfalse;
    return 0.0f;
}

void VayuAudioProcessor::setParameterValue(juce::AudioProcessorValueTreeState& state,
                                              const juce::String& parameterId,
                                              float newValue)
{
    if (auto* parameter = state.getParameter(parameterId))
    {
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost(parameter->convertTo0to1(newValue));
        parameter->endChangeGesture();
    }
}

juce::File VayuAudioProcessor::getUserPresetRootDirectory()
{
    auto directory = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                         .getChildFile("Vayu")
                         .getChildFile("Presets");
    directory.createDirectory();
    return directory;
}

juce::File VayuAudioProcessor::getUserRackPresetDirectory()
{
    auto directory = getUserPresetRootDirectory().getChildFile("Rack");
    directory.createDirectory();
    return directory;
}

juce::File VayuAudioProcessor::getUserEffectPresetDirectory(EffectSlot effectSlot)
{
    auto directory = getUserPresetRootDirectory().getChildFile("Effects").getChildFile(getEffectFolderName(effectSlot));
    directory.createDirectory();
    return directory;
}

void VayuAudioProcessor::updateProcessingState()
{
    if (!processingStateNeedsUpdate.exchange(false))
        return;

    const auto sampleRate = currentSampleRate > 0.0 ? currentSampleRate : getSampleRate();
    if (sampleRate <= 0.0)
    {
        processingStateNeedsUpdate.store(true);
        return;
    }

    auto lowShelfCoefficients = makeLowShelf(sampleRate,
                                             clampFilterFrequency(sampleRate, getParameterValue(parameters, lowShelfFreqId)),
                                             getParameterValue(parameters, lowShelfQId),
                                             getParameterValue(parameters, lowShelfGainId));

    auto lowMidCoefficients = makePeak(sampleRate,
                                       clampFilterFrequency(sampleRate, getParameterValue(parameters, lowMidFreqId)),
                                       getParameterValue(parameters, lowMidQId),
                                       getParameterValue(parameters, lowMidGainId));

    auto midCoefficients = makePeak(sampleRate,
                                    clampFilterFrequency(sampleRate, getParameterValue(parameters, midFreqId)),
                                    getParameterValue(parameters, midQId),
                                    getParameterValue(parameters, midGainId));

    auto highMidCoefficients = makePeak(sampleRate,
                                        clampFilterFrequency(sampleRate, getParameterValue(parameters, highMidFreqId)),
                                        getParameterValue(parameters, highMidQId),
                                        getParameterValue(parameters, highMidGainId));

    auto highShelfCoefficients = makeHighShelf(sampleRate,
                                               clampFilterFrequency(sampleRate, getParameterValue(parameters, highShelfFreqId)),
                                               getParameterValue(parameters, highShelfQId),
                                               getParameterValue(parameters, highShelfGainId));

    const auto outputGain = juce::Decibels::decibelsToGain(getParameterValue(parameters, outputGainId));
    const auto chorusRate = getParameterValue(parameters, chorusRateId);
    const auto chorusDepth = getParameterValue(parameters, chorusDepthId);
    const auto chorusMix = getParameterValue(parameters, chorusMixId);
    const auto flangerRate = getParameterValue(parameters, flangerRateId);
    const auto flangerDepth = getParameterValue(parameters, flangerDepthId);
    const auto flangerMode = static_cast<int>(std::round(getParameterValue(parameters, flangerModeId)));
    const auto phaserRate = getParameterValue(parameters, phaserRateId);
    const auto phaserDepth = getParameterValue(parameters, phaserDepthId);
    const auto phaserMix = getParameterValue(parameters, phaserMixId);
    const auto phaserMode = static_cast<int>(std::round(getParameterValue(parameters, phaserModeId)));

    for (auto& chain : filterChains)
    {
        *chain.get<0>().coefficients = *lowShelfCoefficients;
        *chain.get<1>().coefficients = *lowMidCoefficients;
        *chain.get<2>().coefficients = *midCoefficients;
        *chain.get<3>().coefficients = *highMidCoefficients;
        *chain.get<4>().coefficients = *highShelfCoefficients;
        chain.get<5>().setGainLinear(outputGain);
    }

    juce::ignoreUnused(chorusRate, chorusDepth, chorusMix);

    for (size_t channel = 0; channel < flangerProcessors.size(); ++channel)
    {
        auto& flanger = flangerProcessors[channel];
        const auto side = channel == 0 ? -1.0f : 1.0f;
        const auto baseRate = flangerRate * (flangerMode == 2 ? 1.18f : (flangerMode == 1 ? 0.96f : 0.72f));
        const auto baseDepth = flangerDepth + (flangerMode == 0 ? 0.22f : (flangerMode == 1 ? 0.32f : 0.40f));
        const auto baseDelayMs = flangerMode == 0 ? 0.55f : (flangerMode == 1 ? 1.15f : 0.28f);
        const auto baseFeedback = flangerMode == 2 ? 0.82f : (flangerMode == 1 ? 0.66f : 0.44f);
        flanger.setRate(juce::jlimit(0.05f, 8.0f, baseRate * (1.0f + (0.08f * side))));
        flanger.setDepth(juce::jlimit(0.0f, 1.0f, baseDepth + (0.04f * side)));
        flanger.setMix(1.0f);
        flanger.setCentreDelay(juce::jlimit(1.0f, 100.0f, baseDelayMs + (0.22f * side)));
        flanger.setFeedback(juce::jlimit(-1.0f, 1.0f, baseFeedback + (0.02f * side)));
    }

    juce::ignoreUnused(phaserRate, phaserDepth, phaserMix, phaserMode);

    const auto reverbMode = static_cast<int>(std::round(getParameterValue(parameters, reverbModeId)));
    reverbParameters.roomSize = getParameterValue(parameters, reverbSizeId);
    reverbParameters.damping = getParameterValue(parameters, reverbDampingId);
    reverbParameters.wetLevel = 1.0f;
    reverbParameters.dryLevel = 0.0f;
    reverbParameters.width = reverbMode == 0 ? 0.72f : 1.0f;
    reverbParameters.freezeMode = 0.0f;
    if (reverbMode == 1)
    {
        reverbParameters.roomSize = juce::jlimit(0.0f, 1.0f, reverbParameters.roomSize * 1.25f);
        reverbParameters.damping = juce::jlimit(0.0f, 1.0f, reverbParameters.damping * 0.85f);
    }
    if (reverbMode == 2)
    {
        reverbParameters.roomSize = juce::jlimit(0.0f, 1.0f, reverbParameters.roomSize * 0.92f);
        reverbParameters.damping = juce::jlimit(0.0f, 1.0f, reverbParameters.damping * 0.55f);
    }
    reverbProcessor.setParameters(reverbParameters);

    const juce::SpinLock::ScopedLockType lock(responseCurveLock);
    responseCurveCoefficients[0] = std::move(lowShelfCoefficients);
    responseCurveCoefficients[1] = std::move(lowMidCoefficients);
    responseCurveCoefficients[2] = std::move(midCoefficients);
    responseCurveCoefficients[3] = std::move(highMidCoefficients);
    responseCurveCoefficients[4] = std::move(highShelfCoefficients);
    responseOutputGain = outputGain;
}

void VayuAudioProcessor::parameterChanged(const juce::String& parameterId, float newValue)
{
    juce::ignoreUnused(parameterId, newValue);
    processingStateNeedsUpdate.store(true);
}

float VayuAudioProcessor::measurePeakLevel(const juce::AudioBuffer<float>& buffer, int channelCount)
{
    auto peak = 0.0f;
    for (auto channel = 0; channel < channelCount; ++channel)
        peak = juce::jmax(peak, buffer.getMagnitude(channel, 0, buffer.getNumSamples()));
    return peak;
}

void VayuAudioProcessor::pushNextAnalyserSample(float sample) noexcept
{
    if (analyserFifoIndex == fftSize)
    {
        std::copy(analyserFifo.begin(), analyserFifo.end(), analyserData.begin());
        generateSpectrumData();
        analyserFifoIndex = 0;
    }
    analyserFifo[static_cast<size_t>(analyserFifoIndex++)] = sample;
}

void VayuAudioProcessor::generateSpectrumData()
{
    analyserWindow.multiplyWithWindowingTable(analyserData.data(), fftSize);
    std::fill(analyserData.begin() + fftSize, analyserData.end(), 0.0f);
    analyserFft.performFrequencyOnlyForwardTransform(analyserData.data());

    std::array<float, 128> nextSpectrum{};
    const auto minFrequency = 20.0f;
    const auto maxFrequency = 20000.0f;
    const auto sampleRate = currentSampleRate > 0.0 ? currentSampleRate : 44100.0;

    for (size_t i = 0; i < nextSpectrum.size(); ++i)
    {
        const auto proportion = static_cast<float>(i) / static_cast<float>(nextSpectrum.size() - 1);
        const auto frequency = std::pow(maxFrequency / minFrequency, proportion) * minFrequency;
        const auto fftIndex = juce::jlimit(0, fftSize / 2 - 1,
                                           static_cast<int>(std::round(frequency * static_cast<double>(fftSize) / sampleRate)));
        const auto magnitudeDb = juce::Decibels::gainToDecibels(analyserData[static_cast<size_t>(fftIndex)] / static_cast<float>(fftSize),
                                                                -100.0f);
        nextSpectrum[i] = juce::jmap(magnitudeDb, -100.0f, 0.0f, 0.0f, 1.0f);
    }

    const juce::SpinLock::ScopedLockType lock(analyserLock);
    for (size_t i = 0; i < spectrumData.size(); ++i)
        spectrumData[i] = juce::jmax(nextSpectrum[i], spectrumData[i] * 0.82f);
}

void VayuAudioProcessor::processDistortion(juce::AudioBuffer<float>& buffer, int channelCount)
{
    const auto driveDb = getParameterValue(parameters, distortionDriveId);
    const auto tone = getParameterValue(parameters, distortionToneId);
    const auto mix = getParameterValue(parameters, distortionMixId);
    const auto mode = static_cast<int>(std::round(getParameterValue(parameters, distortionModeId)));
    const auto drive = juce::Decibels::decibelsToGain(driveDb + (mode == 2 ? 8.0f : (mode == 1 ? 4.0f : 0.0f)));
    const auto cutoff = juce::jmap(tone, 900.0f, 14000.0f);
    const auto smoothing = std::exp(-2.0 * juce::MathConstants<double>::pi * cutoff / juce::jmax(1.0, currentSampleRate));
    const auto lowpassCoeff = static_cast<float>(1.0 - smoothing);

    for (int channel = 0; channel < channelCount; ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);
        auto toneState = distortionToneState[static_cast<size_t>(channel)];

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const auto dry = samples[sample];
            auto driven = dry * drive;

            switch (mode)
            {
                case 1:
                    driven = (std::atan(driven * 1.6f) * (2.0f / juce::MathConstants<float>::pi)) + (0.12f * dry);
                    break;
                case 2:
                    driven = juce::jlimit(-0.55f, 0.55f, driven);
                    driven = (driven - (driven * driven * driven * 0.28f)) * 1.28f;
                    break;
                default:
                    driven = std::tanh(driven);
                    break;
            }

            toneState += lowpassCoeff * (driven - toneState);
            samples[sample] = dry + ((toneState - dry) * juce::jlimit(0.0f, 1.0f, mix + (mode == 2 ? 0.15f : 0.0f)));
        }

        distortionToneState[static_cast<size_t>(channel)] = toneState;
    }
}

void VayuAudioProcessor::processChorus(juce::AudioBuffer<float>& buffer, int channelCount)
{
    if (channelCount <= 0 || chorusDelayBuffers[0].empty() || currentSampleRate <= 0.0)
        return;

    if (modulationScratchBuffer.getNumChannels() < channelCount || modulationScratchBuffer.getNumSamples() < buffer.getNumSamples())
        modulationScratchBuffer.setSize(channelCount, buffer.getNumSamples(), false, false, true);

    modulationScratchBuffer.clear();

    const auto mode = static_cast<int>(std::round(getParameterValue(parameters, chorusModeId)));
    const auto depth = getParameterValue(parameters, chorusDepthId);
    const auto rate = getParameterValue(parameters, chorusRateId);
    const auto wetAmount = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters, chorusMixId) + (mode == 2 ? 0.10f : (mode == 1 ? 0.06f : 0.02f)));
    const auto voiceCount = mode == 1 ? 3 : 2;
    const auto baseDelayMs = mode == 0 ? 16.0f : (mode == 1 ? 22.0f : 28.0f);
    const auto modDepthMs = mode == 0 ? (2.0f + (depth * 3.2f)) : (mode == 1 ? (4.5f + (depth * 6.0f)) : (1.2f + (depth * 2.2f)));
    const auto stereoSpreadMs = mode == 2 ? 7.5f : (mode == 1 ? 5.0f : 3.0f);
    const auto voiceSpacingMs = mode == 1 ? 2.8f : 1.8f;
    const auto rateScale = mode == 2 ? 0.38f : (mode == 1 ? 0.62f : 0.48f);
    const auto phaseIncrement = (twoPi * juce::jlimit(0.02f, 6.0f, rate * rateScale)) / static_cast<float>(currentSampleRate);
    auto localPhase = chorusPhase;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        for (int channel = 0; channel < channelCount; ++channel)
        {
            const auto channelIndex = static_cast<size_t>(juce::jmin(channel, static_cast<int>(stereoChannels) - 1));
            const auto dry = buffer.getSample(channel, sample);
            chorusDelayBuffers[channelIndex][static_cast<size_t>(chorusWritePosition)] = dry;
        }

        for (int channel = 0; channel < channelCount; ++channel)
        {
            const auto channelIndex = static_cast<size_t>(juce::jmin(channel, static_cast<int>(stereoChannels) - 1));
            const auto stereoSign = channel == 0 ? -1.0f : 1.0f;
            auto wet = 0.0f;

            for (int voice = 0; voice < voiceCount; ++voice)
            {
                const auto voicePhase = localPhase + (static_cast<float>(voice) * (twoPi / static_cast<float>(voiceCount))) + (stereoSign * 0.35f);
                const auto lfo = std::sin(voicePhase);
                const auto voiceOffsetMs = (static_cast<float>(voice) - (static_cast<float>(voiceCount - 1) * 0.5f)) * voiceSpacingMs;
                const auto delayMs = juce::jlimit(6.0f,
                                                  40.0f,
                                                  baseDelayMs + voiceOffsetMs + (stereoSign * stereoSpreadMs) + (lfo * modDepthMs));
                wet += readInterpolatedSample(chorusDelayBuffers[channelIndex], chorusWritePosition, delayMs * static_cast<float>(currentSampleRate) / 1000.0f);
            }

            wet /= static_cast<float>(voiceCount);
            if (channelCount > 1)
            {
                const auto oppositeIndex = static_cast<size_t>(channel == 0 ? 1 : 0);
                const auto oppositeWet = readInterpolatedSample(chorusDelayBuffers[oppositeIndex], chorusWritePosition,
                                                                (baseDelayMs + (stereoSign * -stereoSpreadMs)) * static_cast<float>(currentSampleRate) / 1000.0f);
                wet += oppositeWet * (mode == 2 ? 0.22f : 0.10f);
            }

            modulationScratchBuffer.setSample(channel, sample, wet);
        }

        localPhase = wrapPhase(localPhase + phaseIncrement);
        chorusWritePosition = (chorusWritePosition + 1) % static_cast<int>(chorusDelayBuffers[0].size());
    }

    chorusPhase = localPhase;
    const auto width = (mode == 2 ? 1.02f : (mode == 1 ? 0.80f : 0.60f)) + (depth * 0.18f);
    blendModulationToStereo(buffer, modulationScratchBuffer, channelCount, wetAmount, width);
}

void VayuAudioProcessor::processFlanger(juce::AudioBuffer<float>& buffer, int channelCount)
{
    if (channelCount <= 0)
        return;

    if (modulationScratchBuffer.getNumChannels() < channelCount || modulationScratchBuffer.getNumSamples() < buffer.getNumSamples())
        modulationScratchBuffer.setSize(channelCount, buffer.getNumSamples(), false, false, true);

    modulationScratchBuffer.makeCopyOf(buffer, true);

    juce::dsp::AudioBlock<float> wetBlock(modulationScratchBuffer);
    for (int channel = 0; channel < channelCount; ++channel)
    {
        auto monoBlock = wetBlock.getSingleChannelBlock(static_cast<size_t>(channel));
        juce::dsp::ProcessContextReplacing<float> context(monoBlock);
        flangerProcessors[static_cast<size_t>(channel)].process(context);
    }

    const auto mode = static_cast<int>(std::round(getParameterValue(parameters, flangerModeId)));
    const auto depth = getParameterValue(parameters, flangerDepthId);
    const auto rate = getParameterValue(parameters, flangerRateId);
    animateStereoField(modulationScratchBuffer,
                       channelCount,
                       flangerPhase,
                       currentSampleRate,
                       rate * (mode == 2 ? 1.06f : (mode == 1 ? 0.88f : 0.66f)),
                       juce::jlimit(0.0f, 1.0f, 0.46f + (depth * 0.86f)),
                       mode == 2 ? 0.10f : (mode == 1 ? 0.06f : 0.04f),
                       mode == 2 ? 0.54f : (mode == 1 ? 0.38f : 0.28f),
                       mode == 2 ? 0.022f : 0.012f);

    const auto wetAmount = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters, flangerMixId) + (mode == 2 ? 0.18f : (mode == 1 ? 0.10f : 0.05f)));
    const auto cancellation = mode == 2 ? 0.92f : (mode == 1 ? 0.74f : 0.56f);
    const auto stereoThrow = mode == 2 ? 0.10f : (mode == 1 ? 0.07f : 0.04f);

    for (int channel = 0; channel < channelCount; ++channel)
    {
        auto* dry = buffer.getWritePointer(channel);
        auto* wet = modulationScratchBuffer.getWritePointer(channel);
        const auto* oppositeWet = modulationScratchBuffer.getReadPointer(channelCount > 1 && channel == 0 ? 1 : 0);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const auto combWet = wet[sample] - (dry[sample] * cancellation)
                               - ((channelCount > 1 ? oppositeWet[sample] : 0.0f) * stereoThrow);
            dry[sample] += combWet * wetAmount;
        }
    }
}

void VayuAudioProcessor::processPhaser(juce::AudioBuffer<float>& buffer, int channelCount)
{
    if (channelCount <= 0 || currentSampleRate <= 0.0)
        return;

    if (modulationScratchBuffer.getNumChannels() < channelCount || modulationScratchBuffer.getNumSamples() < buffer.getNumSamples())
        modulationScratchBuffer.setSize(channelCount, buffer.getNumSamples(), false, false, true);

    modulationScratchBuffer.makeCopyOf(buffer, true);

    const auto mode = static_cast<int>(std::round(getParameterValue(parameters, phaserModeId)));
    const auto depth = getParameterValue(parameters, phaserDepthId);
    const auto rate = getParameterValue(parameters, phaserRateId);
    const auto wetAmount = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters, phaserMixId) + (mode == 2 ? 0.10f : (mode == 1 ? 0.06f : 0.02f)));
    const auto stageCount = mode == 0 ? 4 : 6;
    const auto baseFrequency = mode == 0 ? 280.0f : (mode == 1 ? 420.0f : 620.0f);
    const auto sweepRange = mode == 2 ? (1.6f + (depth * 2.2f)) : (mode == 1 ? (1.2f + (depth * 1.6f)) : (0.8f + (depth * 1.2f)));
    const auto feedbackAmount = mode == 2 ? (0.55f + (depth * 0.20f)) : (mode == 1 ? (0.34f + (depth * 0.18f)) : (0.12f + (depth * 0.10f)));
    const auto phaseIncrement = (twoPi * juce::jlimit(0.02f, 5.0f, rate * (mode == 2 ? 0.80f : (mode == 1 ? 0.62f : 0.46f)))) / static_cast<float>(currentSampleRate);
    auto localPhase = phaserPhase;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        for (int channel = 0; channel < channelCount; ++channel)
        {
            const auto channelIndex = static_cast<size_t>(juce::jmin(channel, static_cast<int>(stereoChannels) - 1));
            const auto stereoOffset = channel == 0 ? -0.28f : 0.28f;
            const auto sweepLfo = std::sin(localPhase + stereoOffset);
            auto stageInput = buffer.getSample(channel, sample) + (phaserFeedbackState[channelIndex] * feedbackAmount);

            for (int stage = 0; stage < stageCount; ++stage)
            {
                const auto stageSpread = 1.0f + (0.18f * static_cast<float>(stage));
                const auto centreFrequency = juce::jlimit(80.0f,
                                                         6000.0f,
                                                         baseFrequency * stageSpread * std::pow(2.0f, sweepLfo * sweepRange));
                const auto tangent = std::tan(juce::MathConstants<float>::pi * centreFrequency / static_cast<float>(currentSampleRate));
                const auto coefficient = (1.0f - tangent) / (1.0f + tangent);
                stageInput = processAllPassSample(stageInput, coefficient, phaserStageState[channelIndex][static_cast<size_t>(stage)]);
            }

            phaserFeedbackState[channelIndex] = stageInput;
            modulationScratchBuffer.setSample(channel, sample, stageInput);
        }

        localPhase = wrapPhase(localPhase + phaseIncrement);
    }

    phaserPhase = localPhase;
    blendModulationToStereo(buffer,
                            modulationScratchBuffer,
                            channelCount,
                            wetAmount,
                            (mode == 2 ? 0.24f : (mode == 1 ? 0.18f : 0.10f)) + (depth * 0.08f));
}

void VayuAudioProcessor::processDelay(juce::AudioBuffer<float>& buffer, int channelCount)
{
    if (channelCount <= 0 || delayBuffers[0].empty())
        return;

    const auto delaySeconds = getParameterValue(parameters, delayTimeId);
    const auto feedback = getParameterValue(parameters, delayFeedbackId);
    const auto mix = getParameterValue(parameters, delayMixId);
    const auto mode = static_cast<int>(std::round(getParameterValue(parameters, delayModeId)));
    const auto modeDelayScale = mode == 0 ? 1.0f : (mode == 1 ? 1.22f : 0.72f);
    const auto delaySamples = juce::jlimit(1, juce::jmax(1, maxDelaySamples - 1),
                                           static_cast<int>(std::round(delaySeconds * modeDelayScale * currentSampleRate)));
    const auto tapeCoeff = mode == 1 ? 0.12f : 0.18f;
    const auto wetMix = juce::jlimit(0.0f, 1.0f, mix + (mode == 2 ? 0.12f : (mode == 1 ? 0.05f : 0.0f)));

    auto* leftSamples = buffer.getWritePointer(0);
    auto* rightSamples = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const auto readPosition = (delayWritePosition - delaySamples + maxDelaySamples) % maxDelaySamples;

        auto delayedLeft = delayBuffers[0][static_cast<size_t>(readPosition)];
        if (mode == 1)
        {
            delayFilterState[0] += tapeCoeff * (delayedLeft - delayFilterState[0]);
            delayedLeft = delayFilterState[0] * 0.88f;
        }

        if (mode == 2 && channelCount > 1)
        {
            auto delayedRight = delayBuffers[1][static_cast<size_t>(readPosition)];
            const auto dryLeft = leftSamples[sample];
            const auto dryRight = rightSamples != nullptr ? rightSamples[sample] : dryLeft;
            const auto monoInput = 0.5f * (dryLeft + dryRight);
            const auto pingPongFeedback = juce::jlimit(0.0f, 0.97f, feedback * 0.96f);
            const auto wetLeft = (delayedLeft * 1.18f) - (delayedRight * 0.08f);
            const auto wetRight = (delayedRight * 1.18f) - (delayedLeft * 0.08f);
            const auto dryCarry = 1.0f - (wetMix * 0.42f);

            delayBuffers[0][static_cast<size_t>(delayWritePosition)] = monoInput + (delayedRight * pingPongFeedback);
            delayBuffers[1][static_cast<size_t>(delayWritePosition)] = delayedLeft * pingPongFeedback;

            leftSamples[sample] = (dryLeft * dryCarry) + (wetLeft * wetMix);
            rightSamples[sample] = (dryRight * dryCarry) + (wetRight * wetMix);
        }
        else
        {
            const auto dryLeft = leftSamples[sample];
            delayBuffers[0][static_cast<size_t>(delayWritePosition)] = dryLeft + (delayedLeft * feedback);
            leftSamples[sample] = dryLeft + ((delayedLeft - dryLeft) * wetMix);

            if (channelCount > 1 && rightSamples != nullptr)
            {
                auto delayedRight = delayBuffers[1][static_cast<size_t>(readPosition)];
                if (mode == 1)
                {
                    delayFilterState[1] += tapeCoeff * (delayedRight - delayFilterState[1]);
                    delayedRight = delayFilterState[1] * 0.88f;
                }

                const auto dryRight = rightSamples[sample];
                delayBuffers[1][static_cast<size_t>(delayWritePosition)] = dryRight + (delayedRight * feedback);
                rightSamples[sample] = dryRight + ((delayedRight - dryRight) * wetMix);
            }
        }

        delayWritePosition = (delayWritePosition + 1) % maxDelaySamples;
    }
}

void VayuAudioProcessor::processStereo(juce::AudioBuffer<float>& buffer, int channelCount)
{
    if (channelCount <= 0)
        return;

    if (modulationScratchBuffer.getNumChannels() < channelCount || modulationScratchBuffer.getNumSamples() < buffer.getNumSamples())
        modulationScratchBuffer.setSize(channelCount, buffer.getNumSamples(), false, false, true);

    modulationScratchBuffer.makeCopyOf(buffer, true);

    const auto mode = static_cast<int>(std::round(getParameterValue(parameters, stereoModeId)));
    const auto rate = getParameterValue(parameters, stereoRateId);
    const auto width = getParameterValue(parameters, stereoDepthId);
    const auto mix = getParameterValue(parameters, stereoMixId);

    animateStereoField(modulationScratchBuffer,
                       channelCount,
                       stereoPhase,
                       currentSampleRate,
                       rate * (mode == 2 ? 1.6f : (mode == 1 ? 1.0f : 0.55f)),
                       juce::jlimit(0.0f, 1.0f, 0.35f + (width * 0.95f)),
                       mode == 0 ? 0.10f : (mode == 1 ? 0.42f : 0.28f),
                       mode == 0 ? 1.10f : (mode == 1 ? 0.62f : 1.25f),
                       mode == 2 ? 0.025f : 0.010f);

    blendModulationToStereo(buffer,
                            modulationScratchBuffer,
                            channelCount,
                            juce::jlimit(0.0f, 1.0f, mix + (mode == 2 ? 0.12f : (mode == 1 ? 0.06f : 0.0f))),
                            0.95f + (width * (mode == 0 ? 0.75f : 1.05f)));
}

void VayuAudioProcessor::processReverb(juce::AudioBuffer<float>& buffer, int channelCount)
{
    if (channelCount <= 0)
        return;

    if (modulationScratchBuffer.getNumChannels() < channelCount || modulationScratchBuffer.getNumSamples() < buffer.getNumSamples())
        modulationScratchBuffer.setSize(channelCount, buffer.getNumSamples(), false, false, true);

    modulationScratchBuffer.makeCopyOf(buffer, true);

    if (channelCount > 1)
        reverbProcessor.processStereo(modulationScratchBuffer.getWritePointer(0), modulationScratchBuffer.getWritePointer(1), modulationScratchBuffer.getNumSamples());
    else
        reverbProcessor.processMono(modulationScratchBuffer.getWritePointer(0), modulationScratchBuffer.getNumSamples());

    const auto mode = static_cast<int>(std::round(getParameterValue(parameters, reverbModeId)));
    const auto wetAmount = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters, reverbMixId) * (mode == 2 ? 0.95f : (mode == 1 ? 0.84f : 0.68f)));
    const auto width = mode == 0 ? 0.45f : (mode == 1 ? 0.78f : 1.0f);
    blendModulationToStereo(buffer, modulationScratchBuffer, channelCount, wetAmount, width);
}

void VayuAudioProcessor::processCompressor(juce::AudioBuffer<float>& buffer,
                                              int channelCount,
                                              const juce::AudioBuffer<float>* sidechainBuffer)
{
    if (channelCount <= 0 || currentSampleRate <= 0.0)
        return;

    const auto thresholdDb = getParameterValue(parameters, compressorThresholdId);
    const auto amount = getParameterValue(parameters, compressorAmountId);
    const auto mix = juce::jlimit(0.0f, 1.0f, getParameterValue(parameters, compressorMixId));
    const auto mode = static_cast<int>(std::round(getParameterValue(parameters, compressorModeId)));

    float ratio = 2.0f + (amount * 3.0f);
    float kneeDb = 6.0f;
    float attackMs = 10.0f;
    float releaseMs = 140.0f;
    float detectorBlend = 0.55f;
    float sidechainHighpassHz = 0.0f;
    float autoMakeupDb = 0.8f + (amount * 1.6f);

    switch (mode)
    {
        case 1: // Glue
            ratio = 2.0f + (amount * 2.0f);
            kneeDb = 11.0f;
            attackMs = 22.0f - (amount * 10.0f);
            releaseMs = 180.0f + (amount * 170.0f);
            detectorBlend = 0.78f;
            sidechainHighpassHz = 90.0f;
            autoMakeupDb = 1.1f + (amount * 1.8f);
            break;
        case 2: // Punch
            ratio = 3.0f + (amount * 4.5f);
            kneeDb = 5.5f;
            attackMs = 18.0f - (amount * 8.0f);
            releaseMs = 85.0f + (amount * 80.0f);
            detectorBlend = 0.25f;
            sidechainHighpassHz = 45.0f;
            autoMakeupDb = 1.0f + (amount * 2.2f);
            break;
        case 3: // Pump
            ratio = 6.0f + (amount * 8.0f);
            kneeDb = 8.0f;
            attackMs = 2.5f - (amount * 1.4f);
            releaseMs = 220.0f + (amount * 300.0f);
            detectorBlend = 0.68f;
            sidechainHighpassHz = 120.0f;
            autoMakeupDb = 1.8f + (amount * 2.8f);
            break;
        default: // Clean
            break;
    }

    const auto attackCoeff = std::exp(-1.0f / (juce::jmax(1.0f, attackMs) * 0.001f * static_cast<float>(currentSampleRate)));
    const auto releaseCoeff = std::exp(-1.0f / (juce::jmax(1.0f, releaseMs) * 0.001f * static_cast<float>(currentSampleRate)));
    const auto gainAttackCoeff = std::exp(-1.0f / (2.0f * 0.001f * static_cast<float>(currentSampleRate)));
    const auto gainReleaseCoeff = std::exp(-1.0f / (55.0f * 0.001f * static_cast<float>(currentSampleRate)));
    const auto rmsCoeff = std::exp(-1.0f / (18.0f * 0.001f * static_cast<float>(currentSampleRate)));
    const auto sidechainCoeff = sidechainHighpassHz > 0.0f
        ? (1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * sidechainHighpassHz / static_cast<float>(currentSampleRate)))
        : 1.0f;
    const auto makeupGain = juce::Decibels::decibelsToGain(autoMakeupDb);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        auto detectorSample = 0.0f;
        if (sidechainBuffer != nullptr && sidechainBuffer->getNumChannels() > 0)
        {
            const auto sidechainChannels = juce::jmin(sidechainBuffer->getNumChannels(), static_cast<int>(stereoChannels));
            for (int channel = 0; channel < sidechainChannels; ++channel)
                detectorSample = juce::jmax(detectorSample, std::abs(sidechainBuffer->getSample(channel, sample)));
        }
        else
        {
            for (int channel = 0; channel < channelCount; ++channel)
                detectorSample = juce::jmax(detectorSample, std::abs(buffer.getSample(channel, sample)));
        }

        if (sidechainHighpassHz > 0.0f)
        {
            compressorSidechainLowpassState += sidechainCoeff * (detectorSample - compressorSidechainLowpassState);
            detectorSample = std::abs(detectorSample - compressorSidechainLowpassState);
        }

        compressorRmsState = (compressorRmsState * rmsCoeff) + ((1.0f - rmsCoeff) * detectorSample * detectorSample);
        const auto rmsSample = std::sqrt(juce::jmax(0.0f, compressorRmsState));
        const auto linkedDetector = juce::jmap(detectorBlend, detectorSample, rmsSample);
        const auto detectorCoeff = linkedDetector > compressorEnvelope ? attackCoeff : releaseCoeff;
        compressorEnvelope = (compressorEnvelope * detectorCoeff) + ((1.0f - detectorCoeff) * linkedDetector);

        const auto envelopeDb = juce::Decibels::gainToDecibels(juce::jmax(0.000001f, compressorEnvelope), -96.0f);
        const auto gainReductionDb = calculateSoftKneeGainReductionDb(envelopeDb, thresholdDb, ratio, kneeDb);
        const auto targetGain = juce::Decibels::decibelsToGain(-gainReductionDb) * makeupGain;
        const auto gainCoeff = targetGain < compressorGainState ? gainAttackCoeff : gainReleaseCoeff;
        compressorGainState = (compressorGainState * gainCoeff) + ((1.0f - gainCoeff) * targetGain);
        compressorGainReductionDb.store(gainReductionDb);

        for (int channel = 0; channel < channelCount; ++channel)
        {
            const auto dry = buffer.getSample(channel, sample);
            const auto wet = dry * compressorGainState;
            buffer.setSample(channel, sample, dry + ((wet - dry) * mix));
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VayuAudioProcessor();
}
