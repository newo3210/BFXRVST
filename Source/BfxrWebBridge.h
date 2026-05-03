#pragma once

#include <JuceHeader.h>

namespace bfxr_web
{
/** Script que expone `__bfxrVstGetActiveSynthParams()` en el contexto de bfxr2 (pestaña Bfxr activa). */
[[nodiscard]] juce::String getBridgeInjectionScript();

/** Interpreta el JSON de `synth.params` de bfxr2 y escribe en el APVTS del motor FigBug (solo parámetros compatibles). */
void applyBfxr2JsonToApvts (const juce::String& jsonUtf8, juce::AudioProcessorValueTreeState& apvts);
} // namespace bfxr_web
