#include "BfxrWebBridge.h"

namespace bfxr_web
{
juce::String getBridgeInjectionScript()
{
    return R"JS(
(function(){
  if (window.__bfxrVstBridgeInstalled) return;
  window.__bfxrVstBridgeInstalled = true;
  window.__bfxrVstGetActiveSynthParams = function(){
    try {
      if (typeof tabs === 'undefined') return null;
      for (var i = 0; i < tabs.length; i++) {
        if (!tabs[i].active) continue;
        var s = tabs[i].synth;
        if (!s || !s.params) continue;
        if (s.name === 'Bfxr') return s.params;
      }
    } catch (e) {}
    return null;
  };
})();
)JS";
}

static int mapWaveTypeJsToFigBug (int jsWave)
{
    if (jsWave >= 0 && jsWave <= 8)
        return jsWave;
    switch (jsWave)
    {
        case 9:
        case 11:
            return 3;
        case 5:
            return 5;
        case 10:
            return 2;
        default:
            return 2;
    }
}

void applyBfxr2JsonToApvts (const juce::String& jsonUtf8, juce::AudioProcessorValueTreeState& apvts)
{
    auto parsed = juce::JSON::parse (jsonUtf8);
    if (! parsed.isObject())
        return;

    const auto* o = parsed.getDynamicObject();
    if (o == nullptr)
        return;

    if (o->hasProperty ("error"))
        return;

    auto setFloat = [&] (const char* id, float v) {
        if (auto* p = apvts.getParameter (id))
            if (auto* pf = dynamic_cast<juce::AudioParameterFloat*> (p))
                pf->setValueNotifyingHost (pf->convertTo0to1 (v));
    };

    auto setInt = [&] (const char* id, int v) {
        if (auto* p = apvts.getParameter (id))
            if (auto* pi = dynamic_cast<juce::AudioParameterInt*> (p))
                pi->setValueNotifyingHost (pi->convertTo0to1 ((float) v));
    };

    auto getNum = [&] (const juce::String& key, float fallback = 0.f) -> float {
        const var v = o->getProperty (juce::Identifier (key));
        if (v.isVoid() || v.isUndefined() || v.isString())
            return fallback;
        return (float) static_cast<double> (v);
    };

    if (o->hasProperty (juce::Identifier ("waveType")))
        setInt ("waveType", mapWaveTypeJsToFigBug (juce::roundToInt (getNum ("waveType", 2.f))));

    setFloat ("masterVolume", getNum ("masterVolume", 0.5f));
    setFloat ("attackTime", getNum ("attackTime"));
    setFloat ("sustainTime", getNum ("sustainTime", 0.3f));
    setFloat ("sustainPunch", getNum ("sustainPunch"));
    setFloat ("decayTime", getNum ("decayTime", 0.4f));
    setFloat ("compressionAmount", getNum ("compressionAmount"));

    setFloat ("startFrequency", getNum ("frequency_start", getNum ("startFrequency", 0.3f)));

    const bool hasFreqSlide = o->hasProperty (juce::Identifier ("frequency_slide"));
    const float slideVal = hasFreqSlide ? (getNum ("frequency_slide") * 2.0f) : getNum ("slide");
    setFloat ("slide", juce::jlimit (-1.f, 1.f, slideVal));
    setFloat ("deltaSlide", getNum ("frequency_acceleration", getNum ("deltaSlide")));
    setFloat ("minFrequency", getNum ("min_frequency_relative_to_starting_frequency", getNum ("minFrequency")));

    setFloat ("vibratoDepth", getNum ("vibratoDepth"));
    setFloat ("vibratoSpeed", getNum ("vibratoSpeed"));

    setFloat ("overtones", getNum ("overtones"));
    setFloat ("overtoneFalloff", getNum ("overtoneFalloff"));

    setFloat ("changeRepeat", getNum ("pitch_jump_repeat_speed", getNum ("changeRepeat")));
    setFloat ("changeAmount", getNum ("pitch_jump_amount", getNum ("changeAmount")));
    setFloat ("changeSpeed", getNum ("pitch_jump_onset_percent", getNum ("changeSpeed")));
    setFloat ("changeAmount2", getNum ("pitch_jump_2_amount", getNum ("changeAmount2")));
    setFloat ("changeSpeed2", getNum ("pitch_jump_onset2_percent", getNum ("changeSpeed2")));

    setFloat ("squareDuty", getNum ("squareDuty"));
    setFloat ("dutySweep", getNum ("dutySweep"));
    setFloat ("repeatSpeed", getNum ("repeatSpeed"));
    setFloat ("flangerOffset", getNum ("flangerOffset"));
    setFloat ("flangerSweep", getNum ("flangerSweep"));
    setFloat ("lpFilterCutoff", getNum ("lpFilterCutoff", 1.f));
    setFloat ("lpFilterCutoffSweep", getNum ("lpFilterCutoffSweep"));
    setFloat ("lpFilterResonance", getNum ("lpFilterResonance"));
    setFloat ("hpFilterCutoff", getNum ("hpFilterCutoff"));
    setFloat ("hpFilterCutoffSweep", getNum ("hpFilterCutoffSweep"));
    setFloat ("bitCrush", getNum ("bitCrush"));
    setFloat ("bitCrushSweep", getNum ("bitCrushSweep"));
}
} // namespace bfxr_web
