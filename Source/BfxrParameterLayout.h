#pragma once

#include <JuceHeader.h>

class SfxrParams;

namespace bfxr
{
[[nodiscard]] juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

void apvtsToSfxr (const juce::AudioProcessorValueTreeState& apvts, SfxrParams& out);

void sfxrToApvts (const SfxrParams& in, juce::AudioProcessorValueTreeState& apvts);

/** Copia valores y locks desde APVTS (solo Randomize/Mutate respetan locks en el motor). */
void prepareSfxrParamsFromApvts (const juce::AudioProcessorValueTreeState& apvts, SfxrParams& out);
} // namespace bfxr
