#pragma once

#include <JuceHeader.h>

namespace bfxr_web
{
[[nodiscard]] juce::String getBridgeInjectionScript();

void applyBfxr2JsonToApvts (const juce::String& jsonUtf8, juce::AudioProcessorValueTreeState& apvts);

[[nodiscard]] juce::String apvtsToBfxr2JsonString (const juce::AudioProcessorValueTreeState& apvts);
} // namespace bfxr_web
