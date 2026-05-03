#pragma once

#include <JuceHeader.h>

#include "BfxrWebPanel.h"
#include "NativeParamsPanel.h"
#include "PluginProcessor.h"

class BfxrAbletonAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit BfxrAbletonAudioProcessorEditor (BfxrAbletonAudioProcessor&);
    ~BfxrAbletonAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::TabbedComponent tabs { juce::TabbedButtonBar::TabsAtTop };
    NativeParamsPanel nativePanel;
    BfxrWebPanel webPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BfxrAbletonAudioProcessorEditor)
};
