#include "PluginEditor.h"
#include "BinaryData.h"

namespace
{
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    constexpr auto lowShelfFreqId = "lowShelfFreq";
    constexpr auto lowShelfGainId = "lowShelfGain";
    constexpr auto lowShelfQId = "lowShelfQ";
    constexpr auto lowMidFreqId = "lowMidFreq";
    constexpr auto lowMidGainId = "lowMidGain";
    constexpr auto lowMidQId = "lowMidQ";
    constexpr auto midFreqId = "midFreq";
    constexpr auto midGainId = "midGain";
    constexpr auto midQId = "midQ";
    constexpr auto highMidFreqId = "highMidFreq";
    constexpr auto highMidGainId = "highMidGain";
    constexpr auto highMidQId = "highMidQ";
    constexpr auto highShelfFreqId = "highShelfFreq";
    constexpr auto highShelfGainId = "highShelfGain";
    constexpr auto highShelfQId = "highShelfQ";
    constexpr auto outputGainId = "outputGain";
    constexpr auto distortionEnabledId = "distortionEnabled";
    constexpr auto distortionModeId = "distortionMode";
    constexpr auto distortionDriveId = "distortionDrive";
    constexpr auto distortionToneId = "distortionTone";
    constexpr auto distortionMixId = "distortionMix";
    constexpr auto chorusEnabledId = "chorusEnabled";
    constexpr auto chorusModeId = "chorusMode";
    constexpr auto chorusRateId = "chorusRate";
    constexpr auto chorusDepthId = "chorusDepth";
    constexpr auto chorusMixId = "chorusMix";
    constexpr auto flangerEnabledId = "flangerEnabled";
    constexpr auto flangerModeId = "flangerMode";
    constexpr auto flangerRateId = "flangerRate";
    constexpr auto flangerDepthId = "flangerDepth";
    constexpr auto flangerMixId = "flangerMix";
    constexpr auto phaserEnabledId = "phaserEnabled";
    constexpr auto phaserModeId = "phaserMode";
    constexpr auto phaserRateId = "phaserRate";
    constexpr auto phaserDepthId = "phaserDepth";
    constexpr auto phaserMixId = "phaserMix";
    constexpr auto delayEnabledId = "delayEnabled";
    constexpr auto delayModeId = "delayMode";
    constexpr auto delayTimeId = "delayTime";
    constexpr auto delayFeedbackId = "delayFeedback";
    constexpr auto delayMixId = "delayMix";
    constexpr auto reverbEnabledId = "reverbEnabled";
    constexpr auto reverbModeId = "reverbMode";
    constexpr auto reverbSizeId = "reverbSize";
    constexpr auto reverbDampingId = "reverbDamping";
    constexpr auto reverbMixId = "reverbMix";
    constexpr auto stereoEnabledId = "stereoEnabled";
    constexpr auto stereoModeId = "stereoMode";
    constexpr auto stereoRateId = "stereoRate";
    constexpr auto stereoDepthId = "stereoDepth";
    constexpr auto stereoMixId = "stereoMix";
    constexpr auto compressorEnabledId = "compressorEnabled";
    constexpr auto compressorModeId = "compressorMode";
    constexpr auto compressorThresholdId = "compressorThreshold";
    constexpr auto compressorAmountId = "compressorAmount";
    constexpr auto compressorMixId = "compressorMix";

    float getParameterValue(const juce::AudioProcessorValueTreeState& state, const juce::String& parameterId)
    {
        if (auto* value = state.getRawParameterValue(parameterId))
            return value->load();
        jassertfalse;
        return 0.0f;
    }

    void setParameterValue(juce::AudioProcessorValueTreeState& state, const juce::String& parameterId, float newValue)
    {
        if (auto* parameter = state.getParameter(parameterId))
            parameter->setValueNotifyingHost(parameter->convertTo0to1(newValue));
    }

    juce::String getPresetDescription(int index)
    {
        static const std::array<const char*, 7> descriptions{{
            "Neutral tuning for starting from scratch.",
            "Adds weight and warmth without muddying the mids.",
            "Pushes intelligibility and upper-mid focus for vocals.",
            "Opens the top end with shine and clarity.",
            "Leans out the lows for a cleaner, tighter contour.",
            "Rounds off the top for darker, softer playback.",
            "Classic smile curve with lifted lows and highs."
        }};
        if (juce::isPositiveAndBelow(index, static_cast<int>(descriptions.size())))
            return descriptions[static_cast<size_t>(index)];
        return "Shape the tone band by band.";
    }

    using EffectSlot = VayuAudioProcessor::EffectSlot;

    constexpr std::array<EffectSlot, 8> effectSlots{{
        EffectSlot::distortion, EffectSlot::chorus, EffectSlot::flanger,
        EffectSlot::phaser, EffectSlot::delay, EffectSlot::stereo, EffectSlot::reverb,
        EffectSlot::compressor
    }};

    juce::String getEffectTitle(EffectSlot slot)
    {
        switch (slot)
        {
            case EffectSlot::distortion: return "Distortion";
            case EffectSlot::chorus: return "Chorus";
            case EffectSlot::flanger: return "Flanger";
            case EffectSlot::phaser: return "Phaser";
            case EffectSlot::delay: return "Delay";
            case EffectSlot::stereo: return "Stereo";
            case EffectSlot::reverb: return "Reverb";
            case EffectSlot::compressor: return "Compressor";
        }
        return {};
    }

    juce::Colour getEffectAccent(EffectSlot slot)
    {
        switch (slot)
        {
            case EffectSlot::distortion: return juce::Colour::fromRGB(146, 255, 191);
            case EffectSlot::chorus: return juce::Colour::fromRGB(116, 239, 255);
            case EffectSlot::flanger: return juce::Colour::fromRGB(88, 212, 255);
            case EffectSlot::phaser: return juce::Colour::fromRGB(139, 255, 204);
            case EffectSlot::delay: return juce::Colour::fromRGB(120, 255, 228);
            case EffectSlot::stereo: return juce::Colour::fromRGB(84, 255, 190);
            case EffectSlot::reverb: return juce::Colour::fromRGB(148, 230, 255);
            case EffectSlot::compressor: return juce::Colour::fromRGB(170, 255, 220);
        }
        return juce::Colours::white;
    }

    std::array<juce::String, 3> getEffectSliderIds(EffectSlot slot)
    {
        switch (slot)
        {
            case EffectSlot::distortion: return { distortionDriveId, distortionToneId, distortionMixId };
            case EffectSlot::chorus: return { chorusRateId, chorusDepthId, chorusMixId };
            case EffectSlot::flanger: return { flangerRateId, flangerDepthId, flangerMixId };
            case EffectSlot::phaser: return { phaserRateId, phaserDepthId, phaserMixId };
            case EffectSlot::delay: return { delayTimeId, delayFeedbackId, delayMixId };
            case EffectSlot::stereo: return { stereoRateId, stereoDepthId, stereoMixId };
            case EffectSlot::reverb: return { reverbSizeId, reverbDampingId, reverbMixId };
            case EffectSlot::compressor: return { compressorThresholdId, compressorAmountId, compressorMixId };
        }
        return {};
    }

    std::array<juce::String, 3> getEffectSliderLabels(EffectSlot slot)
    {
        switch (slot)
        {
            case EffectSlot::distortion: return { "Drive", "Tone", "Mix" };
            case EffectSlot::chorus: return { "Rate", "Depth", "Mix" };
            case EffectSlot::flanger: return { "Rate", "Depth", "Mix" };
            case EffectSlot::phaser: return { "Rate", "Depth", "Mix" };
            case EffectSlot::delay: return { "Time", "Feedback", "Mix" };
            case EffectSlot::stereo: return { "Rate", "Width", "Mix" };
            case EffectSlot::reverb: return { "Size", "Damp", "Mix" };
            case EffectSlot::compressor: return { "Thresh", "Amount", "Mix" };
        }
        return {};
    }

    juce::String getEffectEnabledId(EffectSlot slot)
    {
        switch (slot)
        {
            case EffectSlot::distortion: return distortionEnabledId;
            case EffectSlot::chorus: return chorusEnabledId;
            case EffectSlot::flanger: return flangerEnabledId;
            case EffectSlot::phaser: return phaserEnabledId;
            case EffectSlot::delay: return delayEnabledId;
            case EffectSlot::stereo: return stereoEnabledId;
            case EffectSlot::reverb: return reverbEnabledId;
            case EffectSlot::compressor: return compressorEnabledId;
        }
        return {};
    }

    juce::String getEffectModeId(EffectSlot slot)
    {
        switch (slot)
        {
            case EffectSlot::distortion: return distortionModeId;
            case EffectSlot::chorus: return chorusModeId;
            case EffectSlot::flanger: return flangerModeId;
            case EffectSlot::phaser: return phaserModeId;
            case EffectSlot::delay: return delayModeId;
            case EffectSlot::stereo: return stereoModeId;
            case EffectSlot::reverb: return reverbModeId;
            case EffectSlot::compressor: return compressorModeId;
        }
        return {};
    }

    juce::StringArray getEffectModes(EffectSlot slot)
    {
        switch (slot)
        {
            case EffectSlot::distortion: return { "Warm", "Crunch", "Clip" };
            case EffectSlot::chorus: return { "Dimension", "Ensemble", "MicroPitch" };
            case EffectSlot::flanger: return { "Tape", "Resonant", "Through-Zero" };
            case EffectSlot::phaser: return { "Script", "4-Stage", "Vibe" };
            case EffectSlot::delay: return { "Digital", "Tape", "Ping Pong" };
            case EffectSlot::stereo: return { "Wide", "Auto Pan", "Orbit" };
            case EffectSlot::reverb: return { "Room", "Hall", "Plate" };
            case EffectSlot::compressor: return { "Clean", "Glue", "Punch", "Pump" };
        }
        return {};
    }

    juce::StringArray getEffectModeDescriptions(EffectSlot slot)
    {
        switch (slot)
        {
            case EffectSlot::distortion: return { "Rounded saturation with a softer top end.", "More edge and mid-forward bite for guitars and synths.", "Harder clipping with a denser, more crushed attitude." };
            case EffectSlot::chorus: return { "Subtle dimensional widening inspired by studio chorus units.", "Thicker ensemble motion with more voices and spread.", "Detuned, glossy modulation for luxe stereo width." };
            case EffectSlot::flanger: return { "Tape-like sweeps with gentler comb filtering.", "Resonant jet sweeps with stronger feedback tone.", "Aggressive through-zero style motion and cancellation." };
            case EffectSlot::phaser: return { "Smooth script-style movement with restrained feedback.", "Deeper phasing with a more obvious notch sweep.", "More vocal, vibe-like motion with stronger resonance." };
            case EffectSlot::delay: return { "Clean repeat structure with clear transient detail.", "Darker, filtered repeats with a softer tape attitude.", "Wide left-right bouncing echoes for spacious movement." };
            case EffectSlot::stereo: return { "Broadens the field while staying mostly centered.", "Adds rhythmic pan motion and width travel.", "More theatrical motion and image orbiting." };
            case EffectSlot::reverb: return { "Compact room ambience for glue and short space.", "Longer hall bloom with more depth and openness.", "Bright plate sheen with a denser top-end tail." };
            case EffectSlot::compressor: return { "Transparent control with balanced attack and release.", "Mix-bus style smoothing for cohesion and glue.", "More assertive transient shaping and forward drive.", "Sidechain-style pumping motion with heavier movement." };
        }
        return {};
    }

    std::array<juce::String, 3> getEffectSliderTooltips(EffectSlot slot)
    {
        switch (slot)
        {
            case EffectSlot::distortion: return { "Set the amount of saturation.", "Shape the brightness of the distortion.", "Blend the distorted signal with the clean tone." };
            case EffectSlot::chorus: return { "Adjust the chorus modulation rate.", "Set how wide the modulation swings.", "Blend chorus with the dry signal." };
            case EffectSlot::flanger: return { "Adjust the flanger sweep speed.", "Set the flanger modulation depth.", "Blend flanger with the dry signal." };
            case EffectSlot::phaser: return { "Adjust the phaser sweep speed.", "Set the strength of the phase sweep.", "Blend phaser with the dry signal." };
            case EffectSlot::delay: return { "Set the delay time in seconds.", "Control how much signal feeds back into the delay.", "Blend delay repeats with the dry signal." };
            case EffectSlot::stereo: return { "Set the movement speed of the stereo field.", "Control the width and travel of the image.", "Blend the stereo motion with the dry signal." };
            case EffectSlot::reverb: return { "Set the size of the virtual space.", "Adjust high-frequency damping in the tail.", "Blend reverb with the dry signal." };
            case EffectSlot::compressor: return { "Set the level where compression begins.", "Control how assertive the selected compressor style behaves.", "Blend compressed signal with the dry signal for parallel compression." };
        }
        return {};
    }

    juce::Font makeFont(float height, int styleFlags = juce::Font::plain)
    {
        return juce::Font(juce::FontOptions(height, styleFlags));
    }

    juce::Colour vayuInk() { return juce::Colour::fromRGB(4, 13, 18); }
    juce::Colour vayuNight() { return juce::Colour::fromRGB(10, 27, 34); }
    juce::Colour vayuDeep() { return juce::Colour::fromRGB(12, 42, 51); }
    juce::Colour vayuPanel() { return juce::Colour::fromRGBA(8, 26, 34, 226); }
    juce::Colour vayuPanelSoft() { return juce::Colour::fromRGBA(14, 40, 48, 214); }
    juce::Colour vayuLine() { return juce::Colour::fromRGBA(175, 255, 235, 58); }
    juce::Colour vayuText() { return juce::Colour::fromRGB(226, 250, 244); }
    juce::Colour vayuTextSoft() { return juce::Colour::fromRGBA(214, 242, 236, 176); }
    juce::Colour vayuMint() { return juce::Colour::fromRGB(108, 255, 214); }
    juce::Colour vayuAqua() { return juce::Colour::fromRGB(104, 228, 255); }
    juce::Colour vayuSeafoam() { return juce::Colour::fromRGB(149, 246, 196); }

    void fillGlassCard(juce::Graphics& g, juce::Rectangle<float> bounds, float cornerSize, juce::Colour edgeColour)
    {
        juce::ColourGradient card(vayuPanelSoft(), bounds.getTopLeft(),
                                  vayuPanel().brighter(0.08f), bounds.getBottomRight(), false);
        card.addColour(0.55, vayuPanel().withAlpha(0.96f));
        g.setGradientFill(card);
        g.fillRoundedRectangle(bounds, cornerSize);

        juce::ColourGradient sheen(juce::Colour::fromRGBA(255, 255, 255, 26), bounds.getX(), bounds.getY(),
                                   juce::Colour::fromRGBA(255, 255, 255, 0), bounds.getRight(), bounds.getCentreY(), false);
        g.setGradientFill(sheen);
        g.fillRoundedRectangle(bounds.reduced(1.0f), cornerSize);

        g.setColour(edgeColour);
        g.drawRoundedRectangle(bounds.reduced(0.5f), cornerSize, 1.1f);
    }

    const juce::Image& getVayuIconImage()
    {
        static const auto image = juce::ImageCache::getFromMemory(BinaryData::Vayu_Icon_png,
                                                                  BinaryData::Vayu_Icon_pngSize);
        return image;
    }

    void drawVayuSigilBadge(juce::Graphics& g, juce::Rectangle<float> bounds, float phase)
    {
        if (bounds.isEmpty())
            return;

        const auto badgeBounds = bounds.reduced(1.0f);
        fillGlassCard(g, badgeBounds, 12.0f, vayuAqua().withAlpha(0.38f));

        juce::ColourGradient aura(vayuMint().withAlpha(0.24f + (0.06f * (0.5f + 0.5f * std::sin(phase * 0.22f)))),
                                  badgeBounds.getCentreX(), badgeBounds.getCentreY() - (2.0f * std::sin(phase * 0.18f)),
                                  juce::Colour::fromRGBA(108, 255, 214, 0),
                                  badgeBounds.getCentreX(), badgeBounds.getBottom(), true);
        aura.addColour(0.52, vayuAqua().withAlpha(0.22f));
        g.setGradientFill(aura);
        g.fillRoundedRectangle(badgeBounds.reduced(2.0f), 10.0f);

        const auto iconBounds = badgeBounds.reduced(3.5f);
        juce::Path clipPath;
        clipPath.addRoundedRectangle(iconBounds, 9.0f);

        {
            juce::Graphics::ScopedSaveState state(g);
            g.reduceClipRegion(clipPath);

            const auto& icon = getVayuIconImage();
            if (icon.isValid())
                g.drawImageWithin(icon,
                                  juce::roundToInt(iconBounds.getX()),
                                  juce::roundToInt(iconBounds.getY()),
                                  juce::roundToInt(iconBounds.getWidth()),
                                  juce::roundToInt(iconBounds.getHeight()),
                                  juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize,
                                  false);
            else
            {
                juce::ColourGradient fallback(vayuMint(), iconBounds.getX(), iconBounds.getY(),
                                              vayuAqua(), iconBounds.getRight(), iconBounds.getBottom(), false);
                g.setGradientFill(fallback);
                g.fillRoundedRectangle(iconBounds, 9.0f);
            }
        }

        g.setColour(juce::Colours::white.withAlpha(0.12f));
        g.drawRoundedRectangle(iconBounds, 9.0f, 1.0f);

        g.setColour(vayuAqua().withAlpha(0.18f + (0.10f * (0.5f + 0.5f * std::sin(phase * 0.24f)))));
        g.drawRoundedRectangle(badgeBounds.reduced(0.8f), 12.0f, 1.8f);

        g.setColour(juce::Colours::white.withAlpha(0.72f));
        for (size_t i = 0; i < 4; ++i)
        {
            const auto orbit = static_cast<float>(i) * 1.14f;
            const auto px = badgeBounds.getCentreX() + (std::sin(phase * 0.31f + orbit) * badgeBounds.getWidth() * 0.26f);
            const auto py = badgeBounds.getCentreY() + (std::cos(phase * 0.27f + orbit * 1.2f) * badgeBounds.getHeight() * 0.22f);
            const auto size = 1.0f + (0.8f * (0.5f + 0.5f * std::sin(phase * 0.36f + orbit)));
            g.fillEllipse(px - size, py - size, size * 2.0f, size * 2.0f);
        }
    }

    juce::String formatLevelDb(float linearGain)
    {
        if (linearGain <= 0.00001f)
            return "-inf dB";

        return juce::String(juce::Decibels::gainToDecibels(linearGain), 1) + " dB";
    }

    int countEnabledEffects(const juce::AudioProcessorValueTreeState& state)
    {
        int enabledCount = 0;
        for (const auto slot : effectSlots)
        {
            if (getParameterValue(state, getEffectEnabledId(slot)) > 0.5f)
                ++enabledCount;
        }
        return enabledCount;
    }

    juce::String buildEffectOrderTooltip(const std::array<int, 8>& order)
    {
        juce::StringArray names;
        for (int effectIndex : order)
            names.add(getEffectTitle(effectSlots[static_cast<size_t>(effectIndex)]));
        return "Current effect order: " + names.joinIntoString(" -> ");
    }

    juce::String buildStatusText(const VayuAudioProcessor& processor)
    {
        const auto sampleRate = processor.getCurrentSampleRateValue();
        const auto effectCount = countEnabledEffects(processor.parameters);
        const auto tailSeconds = processor.getTailLengthSeconds();
        const auto compressorEnabled = getParameterValue(processor.parameters, compressorEnabledId) > 0.5f;
        const auto gainReductionDb = processor.getCompressorGainReductionDb();

        juce::StringArray segments;
        if (sampleRate > 0.0)
            segments.add(juce::String(sampleRate / 1000.0, sampleRate >= 96000.0 ? 0 : 1) + " kHz");

        segments.add(effectCount == 0 ? "Clean chain" : juce::String(effectCount) + " FX active");
        if (compressorEnabled && gainReductionDb > 0.1f)
            segments.add("Comp " + juce::String(gainReductionDb, 1) + " dB GR");
        segments.add(tailSeconds > 0.05 ? "Tail " + juce::String(tailSeconds, 1) + " s" : "Instant tail");

        return segments.joinIntoString("  •  ");
    }
}

// ==============================================================================
// EQ Response Curve Component (with spectrum overlay)
// ==============================================================================
class VayuResponseCurveComponent final : public juce::Component,
                                            public juce::SettableTooltipClient,
                                            private juce::Timer
{
public:
    explicit VayuResponseCurveComponent(VayuAudioProcessor& processorToUse)
        : processor(processorToUse) { startTimerHz(24); }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        fillGlassCard(g, bounds, 24.0f, vayuAqua().withAlpha(0.18f));

        juce::ColourGradient halo(juce::Colour::fromRGBA(102, 255, 214, 52), bounds.getX(), bounds.getBottom(),
                                  juce::Colour::fromRGBA(98, 221, 255, 0), bounds.getCentreX(), bounds.getY(), false);
        g.setGradientFill(halo);
        g.fillRoundedRectangle(bounds.reduced(2.0f), 24.0f);

        auto graph = bounds.reduced(18.0f, 18.0f);
        drawGrid(g, graph);
        drawSpectrum(g, graph);
        drawResponse(g, graph);
        drawBandMarkers(g, graph);
        drawLabels(g, graph);
    }

    void mouseDown(const juce::MouseEvent& event) override
    {
        auto graphBounds = getGraphBounds();
        const auto bandIndex = findClosestBand(event.position, graphBounds);
        if (bandIndex < 0) return;
        activeBandIndex = bandIndex;
        dragStartPosition = event.position;
        startingQValue = getParameterValue(processor.parameters, qIds[static_cast<size_t>(activeBandIndex)]);
        beginGestureForBand(activeBandIndex);
        updateBandFromPosition(event.position, graphBounds);
    }

    void mouseDrag(const juce::MouseEvent& event) override
    {
        if (activeBandIndex < 0) return;
        updateBandFromPosition(event.position, getGraphBounds());
    }

    void mouseUp(const juce::MouseEvent&) override
    {
        endGestureForBand(activeBandIndex);
        activeBandIndex = -1;
    }

    void mouseDoubleClick(const juce::MouseEvent& event) override
    {
        const auto bandIndex = findClosestBand(event.position, getGraphBounds());
        if (bandIndex < 0) return;
        resetBandToDefault(bandIndex);
    }

private:
    VayuAudioProcessor& processor;
    int activeBandIndex = -1;
    juce::Point<float> dragStartPosition;
    float startingQValue = 1.0f;

    void timerCallback() override { repaint(); }

    static constexpr std::array<const char*, 5> frequencyIds{ lowShelfFreqId, lowMidFreqId, midFreqId, highMidFreqId, highShelfFreqId };
    static constexpr std::array<const char*, 5> gainIds{ lowShelfGainId, lowMidGainId, midGainId, highMidGainId, highShelfGainId };
    static constexpr std::array<const char*, 5> qIds{ lowShelfQId, lowMidQId, midQId, highMidQId, highShelfQId };
    static constexpr std::array<float, 5> defaultFrequencies{ 110.0f, 260.0f, 1000.0f, 3600.0f, 9200.0f };
    static constexpr std::array<float, 5> defaultGains{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    static constexpr std::array<float, 5> defaultQs{ 0.75f, 0.90f, 1.00f, 0.90f, 0.75f };

    juce::Rectangle<float> getGraphBounds() const { return getLocalBounds().toFloat().reduced(18.0f, 18.0f); }

    static float mapFrequencyToX(float frequency, juce::Rectangle<float> bounds)
    {
        const auto minLog = std::log10(20.0f), maxLog = std::log10(20000.0f);
        const auto position = (std::log10(frequency) - minLog) / (maxLog - minLog);
        return bounds.getX() + bounds.getWidth() * position;
    }

    static float mapXToFrequency(float x, juce::Rectangle<float> bounds)
    {
        const auto proportion = juce::jlimit(0.0f, 1.0f, (x - bounds.getX()) / juce::jmax(1.0f, bounds.getWidth()));
        return std::pow(10.0f, juce::jmap(proportion, std::log10(20.0f), std::log10(20000.0f)));
    }

    static float mapGainToY(float gainDb, juce::Rectangle<float> bounds)
    {
        constexpr auto minDb = -18.0f, maxDb = 18.0f;
        return juce::jmap(gainDb, minDb, maxDb, bounds.getBottom(), bounds.getY());
    }

    static float mapYToGain(float y, juce::Rectangle<float> bounds)
    {
        constexpr auto minDb = -18.0f, maxDb = 18.0f;
        return juce::jmap(juce::jlimit(bounds.getY(), bounds.getBottom(), y), bounds.getBottom(), bounds.getY(), minDb, maxDb);
    }

    static juce::String formatFrequency(float frequency)
    {
        return frequency >= 1000.0f ? juce::String(frequency / 1000.0f, frequency >= 10000.0f ? 0 : 1) + "k"
                                    : juce::String(static_cast<int>(frequency));
    }

    int findClosestBand(juce::Point<float> position, juce::Rectangle<float> bounds) const
    {
        auto bestDistance = std::numeric_limits<float>::max();
        auto bestIndex = -1;
        for (size_t i = 0; i < frequencyIds.size(); ++i)
        {
            const auto frequency = getParameterValue(processor.parameters, frequencyIds[i]);
            const auto gain = getParameterValue(processor.parameters, gainIds[i]);
            const auto bandPoint = juce::Point<float>(mapFrequencyToX(frequency, bounds), mapGainToY(gain, bounds));
            const auto distance = bandPoint.getDistanceFrom(position);
            if (distance < bestDistance) { bestDistance = distance; bestIndex = static_cast<int>(i); }
        }
        return bestDistance <= 28.0f ? bestIndex : -1;
    }

    void beginGestureForBand(int bandIndex)
    {
        if (!juce::isPositiveAndBelow(bandIndex, static_cast<int>(frequencyIds.size()))) return;
        if (auto* p = processor.parameters.getParameter(frequencyIds[static_cast<size_t>(bandIndex)])) p->beginChangeGesture();
        if (auto* p = processor.parameters.getParameter(gainIds[static_cast<size_t>(bandIndex)])) p->beginChangeGesture();
        if (auto* p = processor.parameters.getParameter(qIds[static_cast<size_t>(bandIndex)])) p->beginChangeGesture();
    }

    void endGestureForBand(int bandIndex)
    {
        if (!juce::isPositiveAndBelow(bandIndex, static_cast<int>(frequencyIds.size()))) return;
        if (auto* p = processor.parameters.getParameter(frequencyIds[static_cast<size_t>(bandIndex)])) p->endChangeGesture();
        if (auto* p = processor.parameters.getParameter(gainIds[static_cast<size_t>(bandIndex)])) p->endChangeGesture();
        if (auto* p = processor.parameters.getParameter(qIds[static_cast<size_t>(bandIndex)])) p->endChangeGesture();
    }

    void updateBandFromPosition(juce::Point<float> position, juce::Rectangle<float> bounds)
    {
        const auto index = static_cast<size_t>(activeBandIndex);
        auto frequency = mapXToFrequency(position.x, bounds);
        auto gain = mapYToGain(position.y, bounds);
        auto qValue = startingQValue;

        switch (activeBandIndex)
        {
            case 0: frequency = juce::jlimit(30.0f, 400.0f, frequency); break;
            case 1: frequency = juce::jlimit(120.0f, 1200.0f, frequency); break;
            case 2: frequency = juce::jlimit(400.0f, 4000.0f, frequency); break;
            case 3: frequency = juce::jlimit(1200.0f, 9000.0f, frequency); break;
            case 4: frequency = juce::jlimit(3000.0f, 18000.0f, frequency); break;
            default: return;
        }

        if (juce::ModifierKeys::getCurrentModifiersRealtime().isAltDown())
        {
            const auto delta = (dragStartPosition.y - position.y) * 0.01f;
            switch (activeBandIndex)
            {
                case 0: case 4: qValue = juce::jlimit(0.4f, 1.4f, startingQValue + delta); break;
                case 1: case 2: case 3: qValue = juce::jlimit(0.3f, 4.0f, startingQValue + delta * 2.4f); break;
                default: break;
            }
            setParameterValue(processor.parameters, qIds[index], qValue);
            return;
        }

        setParameterValue(processor.parameters, frequencyIds[index], frequency);
        setParameterValue(processor.parameters, gainIds[index], gain);
    }

    void resetBandToDefault(int bandIndex)
    {
        if (!juce::isPositiveAndBelow(bandIndex, static_cast<int>(frequencyIds.size()))) return;
        const auto index = static_cast<size_t>(bandIndex);
        setParameterValue(processor.parameters, frequencyIds[index], defaultFrequencies[index]);
        setParameterValue(processor.parameters, gainIds[index], defaultGains[index]);
        setParameterValue(processor.parameters, qIds[index], defaultQs[index]);
    }

    void drawGrid(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        g.setColour(vayuLine());
        for (const auto gain : { -12.0f, -6.0f, 0.0f, 6.0f, 12.0f })
        {
            const auto y = mapGainToY(gain, bounds);
            g.drawHorizontalLine(static_cast<int>(std::round(y)), bounds.getX(), bounds.getRight());
        }
        for (const auto freq : { 20.0f, 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f })
        {
            const auto x = mapFrequencyToX(static_cast<float>(freq), bounds);
            g.drawVerticalLine(static_cast<int>(std::round(x)), bounds.getY(), bounds.getBottom());
        }
    }

    void drawSpectrum(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        std::array<float, 128> spectrum;
        processor.copySpectrumData(spectrum);
        juce::Path spectrumPath;
        bool firstPoint = true;
        for (size_t i = 0; i < spectrum.size(); ++i)
        {
            const auto proportion = static_cast<float>(i) / static_cast<float>(spectrum.size() - 1);
            const auto frequency = 20.0f * std::pow(20000.0f / 20.0f, proportion);
            const auto x = mapFrequencyToX(frequency, bounds);
            const auto y = juce::jmap(spectrum[i], 0.0f, 1.0f, bounds.getBottom(), bounds.getY());
            if (firstPoint) { spectrumPath.startNewSubPath(x, y); firstPoint = false; }
            else spectrumPath.lineTo(x, y);
        }
        spectrumPath.lineTo(bounds.getRight(), bounds.getBottom());
        spectrumPath.lineTo(bounds.getX(), bounds.getBottom());
        spectrumPath.closeSubPath();
        g.setColour(vayuAqua().withAlpha(0.12f));
        g.fillPath(spectrumPath);
        g.setColour(vayuAqua().withAlpha(0.72f));
        g.strokePath(spectrumPath, juce::PathStrokeType(1.2f));
    }

    void drawResponse(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        juce::Path response, fill;
        const auto left = bounds.getX(), bottom = bounds.getBottom();
        fill.startNewSubPath(left, bottom);
        for (int x = 0; x < static_cast<int>(bounds.getWidth()); ++x)
        {
            const auto proportion = static_cast<float>(x) / juce::jmax(1.0f, bounds.getWidth() - 1.0f);
            const auto frequency = std::pow(10.0f, juce::jmap(proportion, std::log10(20.0f), std::log10(20000.0f)));
            const auto magnitude = processor.getMagnitudeResponseAtFrequency(frequency);
            const auto gainDb = juce::Decibels::gainToDecibels(magnitude, -24.0f);
            const auto point = juce::Point<float>(left + static_cast<float>(x), mapGainToY(gainDb, bounds));
            if (x == 0) { response.startNewSubPath(point); fill.lineTo(point); }
            else { response.lineTo(point); fill.lineTo(point); }
        }
        fill.lineTo(bounds.getRight(), bottom); fill.closeSubPath();
        juce::ColourGradient glow(vayuMint().withAlpha(0.14f), bounds.getCentreX(), bounds.getY(),
                                  vayuAqua().withAlpha(0.0f), bounds.getCentreX(), bounds.getBottom(), false);
        g.setGradientFill(glow); g.fillPath(fill);
        g.setColour(vayuText().withAlpha(0.18f));
        g.strokePath(response, juce::PathStrokeType(5.0f));
        juce::ColourGradient line(vayuMint(), bounds.getX(), bounds.getY(),
                                  vayuAqua(), bounds.getRight(), bounds.getBottom(), false);
        line.addColour(0.52, vayuSeafoam());
        g.setGradientFill(line);
        g.strokePath(response, juce::PathStrokeType(2.2f, juce::PathStrokeType::JointStyle::curved, juce::PathStrokeType::EndCapStyle::rounded));
    }

    void drawBandMarkers(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        const std::array<juce::Colour, 5> accents{{
            juce::Colour::fromRGB(115, 255, 220), juce::Colour::fromRGB(108, 255, 193),
            juce::Colour::fromRGB(92, 238, 255), juce::Colour::fromRGB(152, 255, 219),
            juce::Colour::fromRGB(116, 224, 255)
        }};
        for (size_t i = 0; i < frequencyIds.size(); ++i)
        {
            const auto freq = getParameterValue(processor.parameters, frequencyIds[i]);
            const auto gain = getParameterValue(processor.parameters, gainIds[i]);
            const auto x = mapFrequencyToX(freq, bounds), y = mapGainToY(gain, bounds);
            const auto colour = accents[i];
            g.setColour(colour.withAlpha(activeBandIndex == static_cast<int>(i) ? 0.34f : 0.22f));
            g.fillEllipse(x - 11.0f, y - 11.0f, 22.0f, 22.0f);
            g.setColour(colour);
            g.fillEllipse(x - 4.5f, y - 4.5f, 9.0f, 9.0f);
            g.setColour(juce::Colours::white.withAlpha(0.65f));
            g.drawEllipse(x - 8.5f, y - 8.5f, 17.0f, 17.0f, 1.2f);
        }
    }

    void drawLabels(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        g.setColour(vayuTextSoft());
        g.setFont(makeFont(12.0f));
        for (const auto gain : { -12.0f, -6.0f, 0.0f, 6.0f, 12.0f })
        {
            const auto y = mapGainToY(gain, bounds);
            g.drawText(juce::String(gain, 0) + " dB", juce::Rectangle<float>(bounds.getX() + 6.0f, y - 8.0f, 48.0f, 16.0f), juce::Justification::left);
        }
        for (const auto freq : { 20.0f, 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f })
        {
            const auto x = mapFrequencyToX(static_cast<float>(freq), bounds);
            g.drawText(formatFrequency(static_cast<float>(freq)), juce::Rectangle<float>(x - 20.0f, bounds.getBottom() - 18.0f, 40.0f, 16.0f), juce::Justification::centred);
        }
    }
};

// ==============================================================================
// Audio Level Meter
// ==============================================================================
class AudioLevelMeter final : public juce::Component,
                              public juce::SettableTooltipClient,
                              private juce::Timer
{
public:
    AudioLevelMeter(juce::String meterName, std::function<float()> levelSupplier)
        : name(std::move(meterName)), supplier(std::move(levelSupplier))
    {
        setInterceptsMouseClicks(false, false);
        setTooltip(name + " level meter");
        startTimerHz(24);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        fillGlassCard(g, bounds, 10.0f, vayuMint().withAlpha(0.15f));

        auto content = bounds.reduced(10.0f, 8.0f);
        auto labelArea = content.removeFromLeft(24.0f);
        auto valueArea = content.removeFromRight(60.0f);
        auto meterArea = content.reduced(8.0f, 8.0f);

        const auto currentLevel = supplier != nullptr ? juce::jlimit(0.0f, 1.4f, supplier()) : 0.0f;
        const auto currentDb = juce::Decibels::gainToDecibels(currentLevel, -60.0f);
        const auto fillProportion = juce::jlimit(0.0f, 1.0f, juce::jmap(currentDb, -60.0f, 3.0f, 0.0f, 1.0f));

        g.setColour(vayuText().withAlpha(0.88f));
        g.setFont(makeFont(11.0f, juce::Font::bold));
        g.drawText(name, labelArea.toNearestInt(), juce::Justification::centredLeft);

        g.setColour(currentLevel > 0.99f ? juce::Colour::fromRGB(255, 146, 120) : vayuTextSoft());
        g.setFont(makeFont(10.0f));
        g.drawText(formatLevelDb(currentLevel), valueArea.toNearestInt(), juce::Justification::centredRight);

        g.setColour(vayuText().withAlpha(0.08f));
        g.fillRoundedRectangle(meterArea, 4.0f);

        auto fillArea = meterArea.withWidth(meterArea.getWidth() * fillProportion);
        juce::ColourGradient fillGradient(vayuMint(), meterArea.getX(), meterArea.getCentreY(),
                                          vayuAqua(), meterArea.getRight(), meterArea.getCentreY(), false);
        fillGradient.addColour(0.78, juce::Colour::fromRGB(255, 162, 98));
        g.setGradientFill(fillGradient);
        g.fillRoundedRectangle(fillArea, 4.0f);
    }

private:
    void timerCallback() override
    {
        repaint();
    }

    juce::String name;
    std::function<float()> supplier;
};

// ==============================================================================
// Band Control Group (Larger knobs)
// ==============================================================================
class VayuBandControlGroup final : public juce::Component
{
public:
    VayuBandControlGroup(juce::AudioProcessorValueTreeState& state,
                            juce::String title, juce::Colour accentColour,
                            const juce::String& freqId, const juce::String& gainId, const juce::String& qId)
        : accent(accentColour)
    {
        const auto tooltipPrefix = title;
        headerLabel.setText(title, juce::dontSendNotification);
        headerLabel.setJustificationType(juce::Justification::centred);
        headerLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        headerLabel.setFont(makeFont(14.0f, juce::Font::bold));
        addAndMakeVisible(headerLabel);

        configureKnob(freqSlider, freqLabel, "Freq", tooltipPrefix + ": adjust the band frequency.");
        configureKnob(gainSlider, gainLabel, "Gain", tooltipPrefix + ": boost or cut this band.");
        configureKnob(qSlider, qLabel, "Q", tooltipPrefix + ": shape how narrow or wide the band feels.");

        setSliderAccent(freqSlider);
        setSliderAccent(gainSlider);
        setSliderAccent(qSlider);

        freqAttachment = std::make_unique<SliderAttachment>(state, freqId, freqSlider);
        gainAttachment = std::make_unique<SliderAttachment>(state, gainId, gainSlider);
        qAttachment = std::make_unique<SliderAttachment>(state, qId, qSlider);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        fillGlassCard(g, bounds, 16.0f, accent.withAlpha(0.42f));

        juce::ColourGradient glow(accent.withAlpha(0.14f), bounds.getCentreX(), bounds.getY(),
                                  juce::Colour::fromRGBA(255, 255, 255, 0), bounds.getCentreX(), bounds.getBottom(), false);
        g.setGradientFill(glow);
        g.fillRoundedRectangle(bounds.reduced(1.5f), 16.0f);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(8, 12);
        headerLabel.setBounds(area.removeFromTop(20));
        area.removeFromTop(6);
        
        const auto knobWidth = area.getWidth() / 3;
        layoutKnob(area.removeFromLeft(knobWidth), freqLabel, freqSlider);
        layoutKnob(area.removeFromLeft(knobWidth), gainLabel, gainSlider);
        layoutKnob(area, qLabel, qSlider);
    }

private:
    juce::Colour accent;
    juce::Label headerLabel;
    juce::Slider freqSlider, gainSlider, qSlider;
    juce::Label freqLabel, gainLabel, qLabel;
    std::unique_ptr<SliderAttachment> freqAttachment, gainAttachment, qAttachment;

    void configureKnob(juce::Slider& slider, juce::Label& label, const juce::String& text, const juce::String& tooltip)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGBA(255, 255, 255, 12));
        slider.setTooltip(tooltip);
        addAndMakeVisible(slider);
        
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, vayuTextSoft());
        label.setFont(makeFont(11.0f));
        label.setTooltip(tooltip);
        addAndMakeVisible(label);
    }

    void setSliderAccent(juce::Slider& slider)
    {
        slider.setColour(juce::Slider::rotarySliderFillColourId, accent);
        slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        slider.setColour(juce::Slider::rotarySliderOutlineColourId, vayuText().withAlpha(0.16f));
    }

    static void layoutKnob(juce::Rectangle<int> area, juce::Label& label, juce::Slider& slider)
    {
        label.setBounds(area.removeFromTop(16));
        slider.setBounds(area.reduced(4, 0));
    }
};

// ==============================================================================
// Effect Parameter Panel (appears when effect is selected)
// ==============================================================================
class EffectParameterPanel : public juce::Component,
                             private juce::Timer
{
public:
    EffectParameterPanel(VayuAudioProcessor& processorToUse)
        : processor(processorToUse), parameters(processorToUse.parameters), compressorReductionBar(compressorReductionValue)
    {
        effectNameLabel.setFont(makeFont(18.0f, juce::Font::bold));
        effectNameLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(effectNameLabel);
        
        bypassButton.setButtonText("Enable");
        bypassButton.setClickingTogglesState(false);
        bypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
        bypassButton.setTooltip("Enable or bypass the selected effect.");
        bypassButton.onClick = [this]
        {
            const auto enabledId = getEffectEnabledId(currentSlot);
            const auto isEnabled = getParameterValue(parameters, enabledId) > 0.5f;
            setParameterValue(parameters, enabledId, isEnabled ? 0.0f : 1.0f);
            refreshCurrentEffectState();
        };
        addAndMakeVisible(bypassButton);

        configurePresetLabel(factoryPresetLabel, "Factory");
        configurePresetLabel(userPresetLabel, "User");

        factoryPresetBox.setColour(juce::ComboBox::backgroundColourId, vayuNight().withAlpha(0.9f));
        factoryPresetBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
        factoryPresetBox.setTooltip("Load a built-in effect preset.");
        factoryPresetBox.onChange = [this]
        {
            if (factoryPresetBox.getSelectedItemIndex() >= 0)
                processor.applyEffectPreset(currentSlot, factoryPresetBox.getSelectedItemIndex());
        };
        addAndMakeVisible(factoryPresetBox);

        userPresetBox.setColour(juce::ComboBox::backgroundColourId, vayuNight().withAlpha(0.9f));
        userPresetBox.setColour(juce::ComboBox::outlineColourId, vayuLine());
        userPresetBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
        userPresetBox.setEditableText(true);
        userPresetBox.setTooltip("Load an existing user preset or type a name to save a new one.");
        userPresetBox.onChange = [this]
        {
            if (userPresetBox.getSelectedId() > 0 && userPresetBox.getText().isNotEmpty())
                processor.loadUserEffectPreset(currentSlot, userPresetBox.getText());
        };
        addAndMakeVisible(userPresetBox);

        savePresetButton.setButtonText("Save");
        savePresetButton.setColour(juce::TextButton::buttonColourId, vayuText().withAlpha(0.08f));
        savePresetButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        savePresetButton.setTooltip("Save the current effect settings as a user preset.");
        savePresetButton.onClick = [this]
        {
            auto name = userPresetBox.getText().trim();
            if (name.isEmpty())
                name = getEffectTitle(currentSlot) + " Patch";

            if (processor.saveUserEffectPreset(currentSlot, name))
            {
                refreshUserPresetBox();
                userPresetBox.setText(name, juce::dontSendNotification);
            }
        };
        addAndMakeVisible(savePresetButton);

        deletePresetButton.setButtonText("Delete");
        deletePresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(34, 82, 84).withAlpha(0.95f));
        deletePresetButton.setColour(juce::TextButton::textColourOffId, juce::Colour::fromRGB(214, 255, 250));
        deletePresetButton.setTooltip("Delete the selected user effect preset from this machine.");
        deletePresetButton.onClick = [this]
        {
            const auto name = userPresetBox.getText().trim();
            if (processor.deleteUserEffectPreset(currentSlot, name))
            {
                refreshUserPresetBox();
                if (userPresetBox.getNumItems() > 0)
                    userPresetBox.setSelectedItemIndex(0, juce::dontSendNotification);
                else
                    userPresetBox.setText({}, juce::dontSendNotification);
            }
        };
        addAndMakeVisible(deletePresetButton);
        
        for (int i = 0; i < 3; ++i)
        {
            sliders[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            sliders[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 65, 18);
            sliders[i].setColour(juce::Slider::thumbColourId, juce::Colours::white);
            sliders[i].setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
            sliders[i].setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
            sliders[i].setColour(juce::Slider::textBoxBackgroundColourId, vayuText().withAlpha(0.08f));
            addAndMakeVisible(sliders[i]);
            
            sliderLabels[i].setJustificationType(juce::Justification::centred);
            sliderLabels[i].setColour(juce::Label::textColourId, vayuTextSoft());
            sliderLabels[i].setFont(makeFont(11.0f));
            addAndMakeVisible(sliderLabels[i]);
        }
        
        modeBox.setColour(juce::ComboBox::backgroundColourId, vayuNight().withAlpha(0.9f));
        modeBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
        modeBox.setTooltip("Change the character of the selected effect.");
        modeBox.onChange = [this] { refreshModeDescription(); };
        addAndMakeVisible(modeBox);

        modeDescriptionLabel.setColour(juce::Label::textColourId, vayuTextSoft());
        modeDescriptionLabel.setFont(makeFont(11.0f, juce::Font::italic));
        modeDescriptionLabel.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(modeDescriptionLabel);

        compressorReductionLabel.setText("Gain Reduction", juce::dontSendNotification);
        compressorReductionLabel.setColour(juce::Label::textColourId, vayuText());
        compressorReductionLabel.setFont(makeFont(11.0f));
        addAndMakeVisible(compressorReductionLabel);

        compressorReductionBar.setColour(juce::ProgressBar::foregroundColourId, vayuMint());
        compressorReductionBar.setColour(juce::ProgressBar::backgroundColourId, vayuText().withAlpha(0.08f));
        compressorReductionBar.setTooltip("Shows live compressor gain reduction.");
        addAndMakeVisible(compressorReductionBar);

        compressorReductionValueLabel.setColour(juce::Label::textColourId, vayuAqua().withAlpha(0.95f));
        compressorReductionValueLabel.setFont(makeFont(11.0f, juce::Font::bold));
        compressorReductionValueLabel.setJustificationType(juce::Justification::centredRight);
        addAndMakeVisible(compressorReductionValueLabel);
        
        setVisible(false);
        startTimerHz(18);
    }
    
    void setEffect(EffectSlot slot)
    {
        currentSlot = slot;
        const auto accent = getEffectAccent(slot);
        effectNameLabel.setText(getEffectTitle(slot), juce::dontSendNotification);
        effectNameLabel.setColour(juce::Label::textColourId, accent);
        bypassButton.setColour(juce::ToggleButton::tickColourId, accent);
        modeBox.setColour(juce::ComboBox::arrowColourId, accent);
        factoryPresetBox.setColour(juce::ComboBox::arrowColourId, accent);
        userPresetBox.setColour(juce::ComboBox::arrowColourId, accent);
        
        modeBox.clear(juce::dontSendNotification);
        modeBox.addItemList(getEffectModes(slot), 1);
        factoryPresetBox.clear(juce::dontSendNotification);
        factoryPresetBox.addItemList(processor.getEffectPresetNames(slot), 1);
        factoryPresetBox.setSelectedItemIndex(-1, juce::dontSendNotification);
        userPresetBox.setText({}, juce::dontSendNotification);
        refreshUserPresetBox();
        
        const auto sliderIds = getEffectSliderIds(slot);
        const auto sliderNames = getEffectSliderLabels(slot);
        const auto sliderTooltips = getEffectSliderTooltips(slot);
        const auto enabledId = getEffectEnabledId(slot);
        const auto modeId = getEffectModeId(slot);
        
        for (int i = 0; i < 3; ++i)
        {
            sliders[i].setColour(juce::Slider::rotarySliderFillColourId, accent);
            sliderLabels[i].setText(sliderNames[i], juce::dontSendNotification);
            sliders[i].setTooltip(sliderTooltips[static_cast<size_t>(i)]);
            sliderLabels[i].setTooltip(sliderTooltips[static_cast<size_t>(i)]);
            sliderAttachments[i] = std::make_unique<SliderAttachment>(parameters, sliderIds[i], sliders[i]);
        }
        
        comboAttachment = std::make_unique<ComboBoxAttachment>(parameters, modeId, modeBox);
        refreshCurrentEffectState();
        refreshModeDescription();

        setVisible(true);
        resized();
        repaint();
    }

    void refreshCurrentEffectState()
    {
        const auto enabledId = getEffectEnabledId(currentSlot);
        const auto isEnabled = getParameterValue(parameters, enabledId) > 0.5f;
        bypassButton.setToggleState(isEnabled, juce::dontSendNotification);
        bypassButton.setButtonText(isEnabled ? "Enabled" : "Bypassed");
        refreshModeDescription();
        refreshCompressorMeter();
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        fillGlassCard(g, bounds, 12.0f, getEffectAccent(currentSlot).withAlpha(0.42f));

        juce::ColourGradient wash(getEffectAccent(currentSlot).withAlpha(0.15f), bounds.getX(), bounds.getY(),
                                  juce::Colour::fromRGBA(255, 255, 255, 0), bounds.getRight(), bounds.getBottom(), false);
        g.setGradientFill(wash);
        g.fillRoundedRectangle(bounds.reduced(1.0f), 12.0f);
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(12, 8);
        
        auto topArea = area.removeFromTop(30);
        effectNameLabel.setBounds(topArea.removeFromLeft(140));
        bypassButton.setBounds(topArea.removeFromRight(88));
        
        area.removeFromTop(10);
        modeBox.setBounds(area.removeFromTop(28));

        area.removeFromTop(6);
        modeDescriptionLabel.setBounds(area.removeFromTop(34));

        area.removeFromTop(10);
        auto factoryArea = area.removeFromTop(28);
        factoryPresetLabel.setBounds(factoryArea.removeFromLeft(48));
        factoryPresetBox.setBounds(factoryArea);

        area.removeFromTop(8);
        auto userArea = area.removeFromTop(28);
        userPresetLabel.setBounds(userArea.removeFromLeft(48));
        savePresetButton.setBounds(userArea.removeFromRight(62));
        userArea.removeFromRight(6);
        deletePresetButton.setBounds(userArea.removeFromRight(72));
        userPresetBox.setBounds(userArea.reduced(0, 0));

        if (currentSlot == EffectSlot::compressor)
        {
            area.removeFromTop(10);
            auto meterArea = area.removeFromTop(22);
            compressorReductionLabel.setBounds(meterArea.removeFromLeft(96));
            compressorReductionValueLabel.setBounds(meterArea.removeFromRight(58));
            compressorReductionBar.setBounds(meterArea.reduced(8, 2));
            area.removeFromTop(8);
        }
        
        area.removeFromTop(12);
        const auto knobWidth = area.getWidth() / 3;
        for (int i = 0; i < 3; ++i)
        {
            auto knobArea = (i == 2) ? area : area.removeFromLeft(knobWidth);
            sliderLabels[i].setBounds(knobArea.removeFromTop(16));
            sliders[i].setBounds(knobArea.reduced(6, 0));
        }
    }
    
private:
    void timerCallback() override
    {
        refreshCompressorMeter();
    }

    void configurePresetLabel(juce::Label& label, const juce::String& text)
    {
        label.setText(text, juce::dontSendNotification);
        label.setColour(juce::Label::textColourId, vayuText());
        label.setFont(makeFont(11.0f, juce::Font::bold));
        label.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(label);
    }

    void refreshUserPresetBox()
    {
        const auto currentText = userPresetBox.getText();
        const auto names = processor.getUserEffectPresetNames(currentSlot);
        userPresetBox.clear(juce::dontSendNotification);
        userPresetBox.addItemList(names, 1);

        if (names.contains(currentText))
            userPresetBox.setText(currentText, juce::dontSendNotification);
        else if (userPresetBox.getNumItems() > 0)
            userPresetBox.setSelectedItemIndex(0, juce::dontSendNotification);
        else
            userPresetBox.setText({}, juce::dontSendNotification);

        deletePresetButton.setEnabled(names.contains(userPresetBox.getText().trim()));
    }

    void refreshModeDescription()
    {
        const auto descriptions = getEffectModeDescriptions(currentSlot);
        const auto modeIndex = juce::jlimit(0, descriptions.size() - 1, modeBox.getSelectedItemIndex());
        modeDescriptionLabel.setText(descriptions.isEmpty() ? juce::String{} : descriptions[modeIndex], juce::dontSendNotification);
    }

    void refreshCompressorMeter()
    {
        const auto isCompressor = currentSlot == EffectSlot::compressor;
        compressorReductionLabel.setVisible(isCompressor);
        compressorReductionBar.setVisible(isCompressor);
        compressorReductionValueLabel.setVisible(isCompressor);

        if (!isCompressor)
            return;

        const auto gainReductionDb = processor.getCompressorGainReductionDb();
        compressorReductionValue = juce::jlimit(0.0, 1.0, static_cast<double>(gainReductionDb) / 18.0);
        compressorReductionValueLabel.setText(juce::String(gainReductionDb, 1) + " dB", juce::dontSendNotification);
        compressorReductionBar.repaint();
    }

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    
    VayuAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& parameters;
    EffectSlot currentSlot = EffectSlot::distortion;
    juce::Label effectNameLabel;
    juce::ToggleButton bypassButton;
    juce::Label factoryPresetLabel;
    juce::ComboBox factoryPresetBox;
    juce::Label userPresetLabel;
    juce::ComboBox userPresetBox;
    juce::TextButton savePresetButton;
    juce::TextButton deletePresetButton;
    std::array<juce::Slider, 3> sliders;
    std::array<juce::Label, 3> sliderLabels;
    juce::ComboBox modeBox;
    juce::Label modeDescriptionLabel;
    juce::Label compressorReductionLabel;
    double compressorReductionValue = 0.0;
    juce::ProgressBar compressorReductionBar;
    juce::Label compressorReductionValueLabel;
    std::array<std::unique_ptr<SliderAttachment>, 3> sliderAttachments;
    std::unique_ptr<ComboBoxAttachment> comboAttachment;
};

// ==============================================================================
// Draggable Effect Button
// ==============================================================================
class DraggableEffectButton : public juce::TextButton
{
public:
    std::function<void()> onDragStart;
    std::function<void(juce::Point<float>)> onDragMove;
    std::function<void(juce::Point<float>)> onDragEnd;
    
    explicit DraggableEffectButton(const juce::String& name)
        : juce::TextButton(name) {}
    
    void mouseDown(const juce::MouseEvent& e) override
    {
        dragStartPosition = e.getScreenPosition().toFloat();
        isDragging = false;
        TextButton::mouseDown(e);
    }
    
    void mouseDrag(const juce::MouseEvent& e) override
    {
        const auto screenPosition = e.getScreenPosition().toFloat();
        if (!isDragging && screenPosition.getDistanceFrom(dragStartPosition) > 6.0f)
        {
            isDragging = true;
            if (onDragStart)
                onDragStart();
        }

        if (isDragging)
        {
            if (onDragMove)
                onDragMove(screenPosition);
            return;
        }

        TextButton::mouseDrag(e);
    }
    
    void mouseUp(const juce::MouseEvent& e) override
    {
        if (isDragging)
        {
            if (onDragEnd)
                onDragEnd(e.getScreenPosition().toFloat());

            isDragging = false;
            dragStartPosition = { -1.0f, -1.0f };
            setState(juce::Button::buttonNormal);
            repaint();
            return;
        }

        dragStartPosition = { -1.0f, -1.0f };
        TextButton::mouseUp(e);
    }
    
private:
    juce::Point<float> dragStartPosition = { -1.0f, -1.0f };
    bool isDragging = false;
};

// ==============================================================================
// Effect Chain Component (Drag to reorder)
// ==============================================================================
class EffectChainComponent : public juce::Component,
                             public juce::SettableTooltipClient
{
public:
    std::function<void(int, int)> onEffectReordered;
    std::function<void(EffectSlot)> onEffectSelected;
    
    EffectChainComponent(juce::AudioProcessorValueTreeState& state)
        : parameters(state)
    {
        effectOrder = { 0, 7, 1, 2, 3, 4, 5, 6 };
        
        for (int i = 0; i < static_cast<int>(effectButtons.size()); ++i)
        {
            int effectIndex = effectOrder[i];
            auto button = std::make_unique<DraggableEffectButton>(getEffectTitle(effectSlots[effectIndex]));
            button->onClick = [this, i] {
                if (onEffectSelected)
                    onEffectSelected(effectSlots[effectOrder[static_cast<size_t>(i)]]);
            };
            button->onDragStart = [this, i] { dragStartIndex = i; };
            button->onDragMove = [this](juce::Point<float> screenPosition) {
                dragHoverIndex = getIndexForScreenPosition(screenPosition);
                repaint();
            };
            button->onDragEnd = [this](juce::Point<float> screenPosition) {
                const auto dropIndex = getIndexForScreenPosition(screenPosition);
                if (dragStartIndex >= 0 && dropIndex >= 0 && dragStartIndex != dropIndex)
                {
                    if (onEffectReordered)
                        onEffectReordered(dragStartIndex, dropIndex);
                }
                dragStartIndex = -1;
                dragHoverIndex = -1;
                repaint();
            };
            effectButtons[i] = std::move(button);
            addAndMakeVisible(effectButtons[i].get());
        }

        refreshButtonState();
        setTooltip(buildEffectOrderTooltip(effectOrder));
    }
    
    void setSelectedEffect(EffectSlot slot)
    {
        selectedSlot = slot;
        refreshButtonState();
    }
    
    void setEffectOrder(const std::array<int, 8>& order)
    {
        effectOrder = order;
        refreshButtonState();
        setTooltip(buildEffectOrderTooltip(effectOrder));
        resized();
        repaint();
    }

    void refreshVisualState()
    {
        refreshButtonState();
        repaint();
    }
    
    std::array<int, 8> getEffectOrder() const { return effectOrder; }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        fillGlassCard(g, bounds, 12.0f, vayuMint().withAlpha(0.18f));
        
        g.setColour(vayuTextSoft());
        g.setFont(makeFont(10.0f, juce::Font::bold));
        g.drawText("DRAG TO REORDER", bounds.reduced(10.0f, 4.0f).removeFromTop(16.0f).toNearestInt(), juce::Justification::centredTop);

        if (dragStartIndex >= 0 && dragHoverIndex >= 0 && dragHoverIndex < static_cast<int>(effectButtons.size()))
        {
            const auto highlightBounds = effectButtons[static_cast<size_t>(dragHoverIndex)]->getBounds().toFloat().expanded(3.0f, 5.0f);
            g.setColour(vayuAqua().withAlpha(0.42f));
            g.drawRoundedRectangle(highlightBounds, 10.0f, 2.0f);
        }
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(4, 20);
        const auto buttonWidth = area.getWidth() / static_cast<int>(effectButtons.size());
        for (int i = 0; i < static_cast<int>(effectButtons.size()); ++i)
        {
            if (i < effectButtons.size() && effectButtons[i] != nullptr)
                effectButtons[i]->setBounds(area.removeFromLeft(buttonWidth).reduced(4, 0));
            else
                area.removeFromLeft(buttonWidth);
        }
    }
    
private:
    int getIndexForScreenPosition(juce::Point<float> screenPosition) const
    {
        const auto localPoint = getLocalPoint(nullptr, screenPosition.toInt());
        for (int i = 0; i < static_cast<int>(effectButtons.size()); ++i)
        {
            if (effectButtons[static_cast<size_t>(i)] != nullptr
                && effectButtons[static_cast<size_t>(i)]->getBounds().expanded(8, 8).contains(localPoint))
                return i;
        }

        return dragStartIndex;
    }

    void refreshButtonState()
    {
        for (int i = 0; i < static_cast<int>(effectButtons.size()); ++i)
        {
            if (effectButtons[static_cast<size_t>(i)] == nullptr)
                continue;

            const auto slot = effectSlots[effectOrder[static_cast<size_t>(i)]];
            const auto isSelected = slot == selectedSlot;
            const auto enabled = getParameterValue(parameters, getEffectEnabledId(slot)) > 0.5f;
            const auto accent = getEffectAccent(slot);

            effectButtons[static_cast<size_t>(i)]->setButtonText(getEffectTitle(slot));
            effectButtons[static_cast<size_t>(i)]->setColour(juce::TextButton::buttonColourId,
                                                             enabled ? accent.withAlpha(isSelected ? 0.92f : 0.42f)
                                                                     : vayuNight().withAlpha(isSelected ? 0.96f : 0.82f));
            effectButtons[static_cast<size_t>(i)]->setColour(juce::TextButton::buttonOnColourId,
                                                             accent.withAlpha(0.95f));
            effectButtons[static_cast<size_t>(i)]->setColour(juce::TextButton::textColourOffId,
                                                             enabled ? juce::Colours::white : vayuTextSoft());
            effectButtons[static_cast<size_t>(i)]->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
            effectButtons[static_cast<size_t>(i)]->setTooltip("Click to edit " + getEffectTitle(slot)
                                                              + (enabled ? " (enabled)." : " (bypassed).")
                                                              + " Drag to move it in the processing chain.");
        }
    }
    
    juce::AudioProcessorValueTreeState& parameters;
    std::array<std::unique_ptr<DraggableEffectButton>, 8> effectButtons;
    std::array<int, 8> effectOrder;
    int dragStartIndex = -1;
    int dragHoverIndex = -1;
    EffectSlot selectedSlot = EffectSlot::distortion;
};

// ==============================================================================
// Help Button
// ==============================================================================
class HelpButton : public juce::TextButton
{
public:
    HelpButton() : juce::TextButton("?")
    {
        setColour(juce::TextButton::buttonColourId, vayuText().withAlpha(0.10f));
        setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        setTooltip("Show tips for using Vayu.");
        onClick = []
        {
            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::NoIcon,
                "Vayu Help",
                "EQ: Drag nodes to adjust frequency/gain. Alt-drag adjusts Q.\n"
                "Double-click a node to reset it.\n\n"
                "Effects: Click any effect button at the bottom to edit its parameters.\n"
                "The parameter panel will appear on the right.\n\n"
                "Compressor: Use Glue for mix-bus tightening, Punch for more transient attitude,\n"
                "and Pump for sidechain-style movement. In plugin hosts, Vayu also supports an optional sidechain input bus.\n\n"
                "Effect Chain: Drag the buttons at the bottom to reorder effects.\n"
                "The audio will process in the order shown.\n\n"
                "Presets: Load factory presets or save your own rack presets.\n\n"
                "Workflow: Use Undo and Redo in the header to quickly step through changes.\n\n"
                "Output: Adjust the master gain.");
        };
    }
};

VayuAudioProcessorEditor::VayuAudioProcessorEditor(VayuAudioProcessor& processorRef)
    : AudioProcessorEditor(&processorRef), audioProcessor(processorRef)
{
    // Title
    titleLabel.setText("Vayu", juce::dontSendNotification);
    titleLabel.setFont(makeFont(28.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, vayuText());
    addAndMakeVisible(titleLabel);
    
    subtitleLabel.setText("Aerodynamic EQ rack for motion, color, and atmosphere", juce::dontSendNotification);
    subtitleLabel.setFont(makeFont(12.0f));
    subtitleLabel.setColour(juce::Label::textColourId, vayuTextSoft());
    addAndMakeVisible(subtitleLabel);

    auto configureHeaderButton = [](juce::TextButton& button, const juce::String& text, const juce::String& tooltip)
    {
        button.setButtonText(text);
        button.setColour(juce::TextButton::buttonColourId, vayuText().withAlpha(0.08f));
        button.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        button.setTooltip(tooltip);
    };

    configureHeaderButton(undoButton, "Undo", "Undo the last parameter or preset change.");
    undoButton.onClick = [this] { audioProcessor.undoLastAction(); };
    addAndMakeVisible(undoButton);

    configureHeaderButton(redoButton, "Redo", "Redo the last undone change.");
    redoButton.onClick = [this] { audioProcessor.redoLastAction(); };
    addAndMakeVisible(redoButton);

    helpButton = std::make_unique<HelpButton>();
    addAndMakeVisible(*helpButton);
    
    // Preset section
    presetLabel.setText("Preset", juce::dontSendNotification);
    presetLabel.setColour(juce::Label::textColourId, vayuText());
    presetLabel.setFont(makeFont(12.0f));
    addAndMakeVisible(presetLabel);
    
    presetBox.setColour(juce::ComboBox::backgroundColourId, vayuNight().withAlpha(0.92f));
    presetBox.setColour(juce::ComboBox::outlineColourId, vayuLine());
    presetBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    presetBox.setColour(juce::ComboBox::arrowColourId, vayuAqua());
    presetBox.setTooltip("Load a factory EQ contour preset.");
    addAndMakeVisible(presetBox);
    populatePresetBox();
    presetBox.onChange = [this] {
        audioProcessor.applyPreset(presetBox.getSelectedItemIndex());
        refreshPresetDescription();
    };
    refreshPresetDescription();
    
    presetDescriptionLabel.setColour(juce::Label::textColourId, vayuTextSoft());
    presetDescriptionLabel.setFont(makeFont(11.0f, juce::Font::italic));
    addAndMakeVisible(presetDescriptionLabel);
    
    // Rack preset section
    rackPresetLabel.setText("Rack", juce::dontSendNotification);
    rackPresetLabel.setColour(juce::Label::textColourId, vayuText());
    rackPresetLabel.setFont(makeFont(12.0f));
    addAndMakeVisible(rackPresetLabel);
    
    rackPresetBox.setColour(juce::ComboBox::backgroundColourId, vayuNight().withAlpha(0.92f));
    rackPresetBox.setColour(juce::ComboBox::outlineColourId, vayuLine());
    rackPresetBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    rackPresetBox.setColour(juce::ComboBox::arrowColourId, vayuMint());
    rackPresetBox.setTooltip("Load a full effect rack preset, including the processing order.");
    addAndMakeVisible(rackPresetBox);
    populateRackPresetBox();
    rackPresetBox.onChange = [this] {
        if (rackPresetBox.getSelectedItemIndex() >= 0)
            audioProcessor.applyRackPreset(rackPresetBox.getSelectedItemIndex());
    };
    
    userRackPresetBox.setColour(juce::ComboBox::backgroundColourId, vayuNight().withAlpha(0.92f));
    userRackPresetBox.setColour(juce::ComboBox::outlineColourId, vayuLine());
    userRackPresetBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    userRackPresetBox.setEditableText(true);
    userRackPresetBox.setTooltip("Load an existing user rack preset or type a name to save a new one.");
    addAndMakeVisible(userRackPresetBox);
    populateUserRackPresetBox();
    userRackPresetBox.onChange = [this] {
        if (userRackPresetBox.getSelectedId() > 0 && userRackPresetBox.getText().isNotEmpty())
            audioProcessor.loadUserRackPreset(userRackPresetBox.getText());
    };
    
    saveRackPresetButton.setButtonText("Save");
    saveRackPresetButton.setColour(juce::TextButton::buttonColourId, vayuText().withAlpha(0.08f));
    saveRackPresetButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    saveRackPresetButton.setTooltip("Save the current rack, including effect order, as a user preset.");
    addAndMakeVisible(saveRackPresetButton);
    saveRackPresetButton.onClick = [this]
    {
        auto name = userRackPresetBox.getText().trim();
        if (name.isEmpty()) name = "My Rack";
        if (audioProcessor.saveUserRackPreset(name))
        {
            populateUserRackPresetBox();
            userRackPresetBox.setText(name, juce::dontSendNotification);
        }
    };

    deleteRackPresetButton.setButtonText("Delete");
    deleteRackPresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(34, 82, 84).withAlpha(0.95f));
    deleteRackPresetButton.setColour(juce::TextButton::textColourOffId, juce::Colour::fromRGB(214, 255, 250));
    deleteRackPresetButton.setTooltip("Delete the selected user rack preset from this machine.");
    addAndMakeVisible(deleteRackPresetButton);
    deleteRackPresetButton.onClick = [this]
    {
        const auto name = userRackPresetBox.getText().trim();
        if (audioProcessor.deleteUserRackPreset(name))
        {
            populateUserRackPresetBox();
            if (userRackPresetBox.getNumItems() > 0)
                userRackPresetBox.setSelectedItemIndex(0, juce::dontSendNotification);
            else
                userRackPresetBox.setText({}, juce::dontSendNotification);
        }
    };

    
    // Output control - hide the text box to remove the "-1.0" ghost
    outputLabel.setText("Out", juce::dontSendNotification);
    outputLabel.setJustificationType(juce::Justification::centred);
    outputLabel.setColour(juce::Label::textColourId, vayuText());
    outputLabel.setFont(makeFont(11.0f));
    addAndMakeVisible(outputLabel);
    
    outputSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    outputSlider.setColour(juce::Slider::rotarySliderFillColourId, vayuMint());
    outputSlider.setColour(juce::Slider::rotarySliderOutlineColourId, vayuText().withAlpha(0.14f));
    outputSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    outputSlider.setTooltip("Set the final output level.");
    addAndMakeVisible(outputSlider);
    outputAttachment = std::make_unique<SliderAttachment>(audioProcessor.parameters, outputGainId, outputSlider);
    inputMeter = std::make_unique<AudioLevelMeter>("In", [this] { return audioProcessor.getInputLevel(); });
    outputMeter = std::make_unique<AudioLevelMeter>("Out", [this] { return audioProcessor.getOutputLevel(); });
    addAndMakeVisible(*inputMeter);
    addAndMakeVisible(*outputMeter);

    statusLabel.setColour(juce::Label::textColourId, vayuTextSoft());
    statusLabel.setFont(makeFont(11.0f));
    statusLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(statusLabel);

    
    // EQ Response Curve
    responseCurve = std::make_unique<VayuResponseCurveComponent>(audioProcessor);
    responseCurve->setTooltip("Drag EQ nodes to shape the response. Alt-drag to adjust Q. Double-click a node to reset it.");
    addAndMakeVisible(*responseCurve);

    
    // Effect Parameter Panel
    effectParameterPanel = std::make_unique<EffectParameterPanel>(audioProcessor);
    addAndMakeVisible(*effectParameterPanel);

    
    // Effect Chain (bottom)
    effectChain = std::make_unique<EffectChainComponent>(audioProcessor.parameters);
    effectChain->setEffectOrder(audioProcessor.getEffectOrder());
    effectChain->onEffectSelected = [this](EffectSlot slot) {
        effectParameterPanel->setEffect(slot);
        effectChain->setSelectedEffect(slot);
    };
    effectChain->onEffectReordered = [this](int from, int to) {
        auto order = effectChain->getEffectOrder();
        const auto movedEffect = order[static_cast<size_t>(from)];
        if (from < to)
        {
            for (int i = from; i < to; ++i)
                order[static_cast<size_t>(i)] = order[static_cast<size_t>(i + 1)];
        }
        else
        {
            for (int i = from; i > to; --i)
                order[static_cast<size_t>(i)] = order[static_cast<size_t>(i - 1)];
        }
        order[static_cast<size_t>(to)] = movedEffect;
        effectChain->setEffectOrder(order);
        audioProcessor.setEffectOrder(order);
    };
    addAndMakeVisible(*effectChain);

    effectParameterPanel->setEffect(EffectSlot::distortion);
    effectChain->setSelectedEffect(EffectSlot::distortion);
    
    // EQ Band Controls
    const std::array<juce::Colour, 5> accents{{
        juce::Colour::fromRGB(115, 255, 220), juce::Colour::fromRGB(108, 255, 193),
        juce::Colour::fromRGB(92, 238, 255), juce::Colour::fromRGB(152, 255, 219),
        juce::Colour::fromRGB(116, 224, 255)
    }};
    
    bandControls[0] = std::make_unique<VayuBandControlGroup>(audioProcessor.parameters, "Low Shelf", accents[0], lowShelfFreqId, lowShelfGainId, lowShelfQId);
    bandControls[1] = std::make_unique<VayuBandControlGroup>(audioProcessor.parameters, "Low Mid", accents[1], lowMidFreqId, lowMidGainId, lowMidQId);
    bandControls[2] = std::make_unique<VayuBandControlGroup>(audioProcessor.parameters, "Mid", accents[2], midFreqId, midGainId, midQId);
    bandControls[3] = std::make_unique<VayuBandControlGroup>(audioProcessor.parameters, "High Mid", accents[3], highMidFreqId, highMidGainId, highMidQId);
    bandControls[4] = std::make_unique<VayuBandControlGroup>(audioProcessor.parameters, "High Shelf", accents[4], highShelfFreqId, highShelfGainId, highShelfQId);
    
    for (auto& control : bandControls)
        addAndMakeVisible(*control);

    
    setResizable(true, true);
    setResizeLimits(1200, 700, 1760, 980);
    setSize(1300, 720);
    refreshStatusText();
    startTimerHz(24);
}

VayuAudioProcessorEditor::~VayuAudioProcessorEditor() = default;

void VayuAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    juce::ColourGradient bg(vayuInk(), 0, 0, vayuDeep(), bounds.getRight(), bounds.getBottom(), false);
    bg.addColour(0.42, vayuNight());
    g.setGradientFill(bg);
    g.fillAll();

    const auto leftOrbOffset = std::sin(animationPhase * 0.8f) * 18.0f;
    const auto rightOrbOffset = std::cos(animationPhase * 0.72f) * 22.0f;

    juce::ColourGradient orbLeft(vayuMint().withAlpha(0.20f), bounds.getX() + 180.0f + leftOrbOffset, bounds.getY() + 120.0f,
                                 juce::Colour::fromRGBA(108, 255, 214, 0), bounds.getX() + 180.0f, bounds.getY() + 380.0f, true);
    g.setGradientFill(orbLeft);
    g.fillEllipse(bounds.getX() - 60.0f, bounds.getY() + 30.0f, 420.0f, 420.0f);

    juce::ColourGradient orbRight(vayuAqua().withAlpha(0.16f), bounds.getRight() - 180.0f + rightOrbOffset, bounds.getY() + 160.0f,
                                  juce::Colour::fromRGBA(104, 228, 255, 0), bounds.getRight() - 120.0f, bounds.getBottom() - 80.0f, true);
    g.setGradientFill(orbRight);
    g.fillEllipse(bounds.getRight() - 420.0f, bounds.getY() - 10.0f, 360.0f, 360.0f);

    juce::Path ribbon;
    ribbon.startNewSubPath(bounds.getX() - 24.0f, bounds.getY() + 138.0f + (std::sin(animationPhase) * 12.0f));
    ribbon.cubicTo(bounds.getX() + bounds.getWidth() * 0.22f, bounds.getY() + 102.0f + (std::cos(animationPhase * 0.9f) * 10.0f),
                   bounds.getX() + bounds.getWidth() * 0.52f, bounds.getY() + 172.0f + (std::sin(animationPhase * 1.2f) * 11.0f),
                   bounds.getRight() + 16.0f, bounds.getY() + 116.0f + (std::cos(animationPhase * 0.7f) * 9.0f));
    g.setColour(vayuAqua().withAlpha(0.08f));
    g.strokePath(ribbon, juce::PathStrokeType(16.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour(vayuMint().withAlpha(0.16f));
    g.strokePath(ribbon, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    g.setColour(vayuText().withAlpha(0.04f));
    for (float y = 24.0f; y < bounds.getBottom(); y += 26.0f)
        g.drawHorizontalLine(static_cast<int>(y), bounds.getX(), bounds.getRight());

    drawVayuSigilBadge(g, sigilBounds.toFloat(), animationPhase);
}

void VayuAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(16, 16);
    
    // Top bar
    auto topBar = area.removeFromTop(42);
    titleLabel.setBounds(topBar.removeFromLeft(92));
    subtitleLabel.setBounds(topBar.removeFromLeft(304));
    redoButton.setBounds(topBar.removeFromRight(64));
    topBar.removeFromRight(6);
    undoButton.setBounds(topBar.removeFromRight(64));
    topBar.removeFromRight(8);
    if (helpButton != nullptr)
        helpButton->setBounds(topBar.removeFromRight(40));
    topBar.removeFromRight(8);
    sigilBounds = topBar.removeFromRight(42);
    
    // Preset row
    auto presetRow = area.removeFromTop(50);
    presetLabel.setBounds(presetRow.removeFromLeft(40));
    presetBox.setBounds(presetRow.removeFromLeft(180));
    presetRow.removeFromLeft(20);
    rackPresetLabel.setBounds(presetRow.removeFromLeft(35));
    rackPresetBox.setBounds(presetRow.removeFromLeft(140));
    userRackPresetBox.setBounds(presetRow.removeFromLeft(140));
    saveRackPresetButton.setBounds(presetRow.removeFromLeft(60));
    presetRow.removeFromLeft(6);
    deleteRackPresetButton.setBounds(presetRow.removeFromLeft(74));
    presetDescriptionLabel.setBounds(area.removeFromTop(22));
    
    area.removeFromTop(10);

    auto chainArea = area.removeFromBottom(84);
    effectChain->setBounds(chainArea);

    area.removeFromBottom(8);

    // Main content: EQ on left, parameter panel on right
    auto mainArea = area;
    auto leftArea = mainArea.removeFromLeft(mainArea.proportionOfWidth(0.70f));
    auto rightArea = mainArea;
    rightArea.removeFromLeft(10);
    
    // EQ Graph
    auto eqArea = leftArea.removeFromTop(juce::jmax(260, leftArea.proportionOfHeight(0.48f)));
    responseCurve->setBounds(eqArea);
    
    leftArea.removeFromTop(10);

    // EQ Band Controls (horizontal row)
    auto utilityArea = leftArea.removeFromBottom(108);
    auto bandArea = leftArea;
    const auto bandWidth = bandArea.getWidth() / 5;
    for (int i = 0; i < 5; ++i)
        bandControls[i]->setBounds(bandArea.removeFromLeft(bandWidth).reduced(3, 0));
    
    // Output and metering
    auto outputArea = utilityArea.removeFromLeft(110);
    outputLabel.setBounds(outputArea.removeFromTop(18));
    outputSlider.setBounds(outputArea.reduced(8, 0));

    utilityArea.removeFromLeft(10);
    auto meterArea = utilityArea.removeFromLeft(240);
    inputMeter->setBounds(meterArea.removeFromTop(36));
    meterArea.removeFromTop(10);
    outputMeter->setBounds(meterArea.removeFromTop(36));

    utilityArea.removeFromLeft(12);
    statusLabel.setBounds(utilityArea.reduced(4, 8));
    
    // Effect parameter panel (right side)
    effectParameterPanel->setBounds(rightArea);
}

void VayuAudioProcessorEditor::populatePresetBox()
{
    const auto names = audioProcessor.getPresetNames();
    for (int i = 0; i < names.size(); ++i)
        presetBox.addItem(names[i], i + 1);
    presetBox.setSelectedItemIndex(audioProcessor.getCurrentPresetIndex(), juce::dontSendNotification);
}

void VayuAudioProcessorEditor::populateRackPresetBox()
{
    const auto names = audioProcessor.getRackPresetNames();
    for (int i = 0; i < names.size(); ++i)
        rackPresetBox.addItem(names[i], i + 1);
    rackPresetBox.setSelectedItemIndex(audioProcessor.getCurrentRackPresetIndex(), juce::dontSendNotification);
}

void VayuAudioProcessorEditor::populateUserRackPresetBox()
{
    const auto currentText = userRackPresetBox.getText();
    userRackPresetBox.clear(juce::dontSendNotification);
    const auto names = audioProcessor.getUserRackPresetNames();
    userRackPresetBox.addItemList(names, 1);
    if (names.contains(currentText))
        userRackPresetBox.setText(currentText, juce::dontSendNotification);
    else if (userRackPresetBox.getNumItems() > 0 && currentText.isEmpty())
        userRackPresetBox.setSelectedItemIndex(0, juce::dontSendNotification);

    deleteRackPresetButton.setEnabled(names.contains(userRackPresetBox.getText().trim()));
}

void VayuAudioProcessorEditor::refreshPresetDescription()
{
    presetDescriptionLabel.setText(getPresetDescription(presetBox.getSelectedItemIndex()), juce::dontSendNotification);
}

void VayuAudioProcessorEditor::refreshStatusText()
{
    statusLabel.setText(buildStatusText(audioProcessor), juce::dontSendNotification);
}

void VayuAudioProcessorEditor::timerCallback()
{
    animationPhase += 0.055f;
    constexpr auto animationLoopLength = juce::MathConstants<float>::twoPi * 100.0f;
    if (animationPhase > animationLoopLength)
        animationPhase -= animationLoopLength;

    undoButton.setEnabled(audioProcessor.canUndo());
    redoButton.setEnabled(audioProcessor.canRedo());

    const auto currentPreset = audioProcessor.getCurrentPresetIndex();
    if (presetBox.getSelectedItemIndex() != currentPreset)
        presetBox.setSelectedItemIndex(currentPreset, juce::dontSendNotification);
    
    const auto currentRackPreset = audioProcessor.getCurrentRackPresetIndex();
    if (rackPresetBox.getSelectedItemIndex() != currentRackPreset)
        rackPresetBox.setSelectedItemIndex(currentRackPreset, juce::dontSendNotification);

    if (effectChain->getEffectOrder() != audioProcessor.getEffectOrder())
        effectChain->setEffectOrder(audioProcessor.getEffectOrder());
    effectChain->refreshVisualState();
    effectParameterPanel->refreshCurrentEffectState();

    if (userRackPresetBox.getNumItems() != audioProcessor.getUserRackPresetNames().size())
        populateUserRackPresetBox();
    else
        deleteRackPresetButton.setEnabled(audioProcessor.getUserRackPresetNames().contains(userRackPresetBox.getText().trim()));
    
    refreshPresetDescription();
    refreshStatusText();
    repaint();
}

void VayuAudioProcessorEditor::parentHierarchyChanged()
{
    AudioProcessorEditor::parentHierarchyChanged();

    if (hasPositionedStandaloneWindow)
        return;

    if (auto* window = findParentComponentOfClass<juce::DocumentWindow>())
    {
        if (auto* display = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay())
        {
            const auto available = display->userArea.reduced(48);
            window->setBounds(available.withSizeKeepingCentre(getWidth(), getHeight()));
            window->toFront(true);
            hasPositionedStandaloneWindow = true;
        }
    }
}

void VayuAudioProcessorEditor::visibilityChanged()
{
    AudioProcessorEditor::visibilityChanged();
}
