#pragma once

#include <JuceHeader.h>

class SfxrParams;

namespace bfxr
{
[[nodiscard]] juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

/** Copia valores del APVTS a SfxrParams (lectura en hilo de audio vía parámetros JUCE). */
void apvtsToSfxr (const juce::AudioProcessorValueTreeState& apvts, SfxrParams& out);

/** Aplica SfxrParams al APVTS (hilo de mensajes / UI). */
void sfxrToApvts (const SfxrParams& in, juce::AudioProcessorValueTreeState& apvts);
} // namespace bfxr
