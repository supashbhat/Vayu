#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class VayuResponseCurveComponent;
class VayuBandControlGroup;
class EffectParameterPanel;
class EffectChainComponent;
class HelpButton;
class AudioLevelMeter;

class VayuAudioProcessorEditor : public juce::AudioProcessorEditor,
                                 private juce::Timer
{
public:
    explicit VayuAudioProcessorEditor(VayuAudioProcessor& processorRef);
    ~VayuAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void parentHierarchyChanged() override;
    void visibilityChanged() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    VayuAudioProcessor& audioProcessor;
    juce::TooltipWindow tooltipWindow { this, 500 };

    // Top bar
    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::TextButton undoButton;
    juce::TextButton redoButton;
    std::unique_ptr<HelpButton> helpButton;

    // Presets
    juce::Label presetLabel;
    juce::ComboBox presetBox;
    juce::Label presetDescriptionLabel;
    juce::Label rackPresetLabel;
    juce::ComboBox rackPresetBox;
    juce::ComboBox userRackPresetBox;
    juce::TextButton saveRackPresetButton;
    juce::TextButton deleteRackPresetButton;

    // Output
    juce::Label outputLabel;
    juce::Slider outputSlider;
    std::unique_ptr<SliderAttachment> outputAttachment;
    std::unique_ptr<AudioLevelMeter> inputMeter;
    std::unique_ptr<AudioLevelMeter> outputMeter;
    juce::Label statusLabel;

    // EQ
    std::unique_ptr<VayuResponseCurveComponent> responseCurve;
    std::array<std::unique_ptr<VayuBandControlGroup>, 5> bandControls;

    // Effect parameters
    std::unique_ptr<EffectParameterPanel> effectParameterPanel;

    // Effect chain (drag to reorder at bottom)
    std::unique_ptr<EffectChainComponent> effectChain;
    bool hasPositionedStandaloneWindow = false;
    juce::Rectangle<int> sigilBounds;
    float animationPhase = 0.0f;

    void populatePresetBox();
    void populateRackPresetBox();
    void populateUserRackPresetBox();
    void refreshPresetDescription();
    void refreshStatusText();
    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VayuAudioProcessorEditor)
};
