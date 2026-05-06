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
  window.__bfxrVstApplyParamsObject = function(p){
    try {
      if (!p || typeof tabs === 'undefined') return 'bad_param';
      for (var i = 0; i < tabs.length; i++) {
        if (!tabs[i].active) continue;
        var s = tabs[i].synth;
        if (!s || !s.params || s.name !== 'Bfxr') continue;
        s.apply_params(p, false);
        tabs[i].update_ui_params();
        return 'ok';
      }
      return 'no_tab';
    } catch (e) { return String(e); }
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
    {
#if JUCE_DEBUG
        DBG ("applyBfxr2JsonToApvts: error desde web — "
             << o->getProperty ("error").toString());
#endif
        return;
    }

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
    {
        const int mappedWt = mapWaveTypeJsToFigBug (juce::roundToInt (getNum ("waveType", 2.f)));
#if JUCE_DEBUG
        jassert (mappedWt >= 0 && mappedWt <= 8);
#endif
        setInt ("waveType", mappedWt);
    }

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

static float readApvtsFloat (const juce::AudioProcessorValueTreeState& apvts, const char* id, float fallback = 0.f)
{
    if (auto* p = apvts.getParameter (id))
        if (auto* pf = dynamic_cast<juce::AudioParameterFloat*> (p))
            return pf->convertFrom0to1 (apvts.getRawParameterValue (id)->load());
    return fallback;
}

static int readApvtsIntWave (const juce::AudioProcessorValueTreeState& apvts)
{
    if (auto* p = apvts.getParameter ("waveType"))
        if (auto* pi = dynamic_cast<juce::AudioParameterInt*> (p))
            return (int) std::lround ((double) pi->convertFrom0to1 (apvts.getRawParameterValue ("waveType")->load()));
    return 2;
}

juce::String apvtsToBfxr2JsonString (const juce::AudioProcessorValueTreeState& apvts)
{
    juce::DynamicObject::Ptr o = new juce::DynamicObject();

    const int wt = readApvtsIntWave (apvts);
    o->setProperty ("waveType", wt);

    auto put = [&] (const char* bfxrKey, const char* apvtsId) {
        o->setProperty (juce::Identifier (bfxrKey), readApvtsFloat (apvts, apvtsId));
    };

    o->setProperty ("masterVolume", readApvtsFloat (apvts, "masterVolume", 0.5f));
    put ("attackTime", "attackTime");
    put ("sustainTime", "sustainTime");
    put ("sustainPunch", "sustainPunch");
    put ("decayTime", "decayTime");
    put ("compressionAmount", "compressionAmount");

    o->setProperty ("frequency_start", readApvtsFloat (apvts, "startFrequency", 0.3f));
    o->setProperty ("frequency_slide", juce::jlimit (-0.5f, 0.5f, readApvtsFloat (apvts, "slide") * 0.5f));
    o->setProperty ("frequency_acceleration", readApvtsFloat (apvts, "deltaSlide"));
    o->setProperty ("min_frequency_relative_to_starting_frequency", readApvtsFloat (apvts, "minFrequency"));

    put ("vibratoDepth", "vibratoDepth");
    put ("vibratoSpeed", "vibratoSpeed");

    put ("overtones", "overtones");
    put ("overtoneFalloff", "overtoneFalloff");

    o->setProperty ("pitch_jump_repeat_speed", readApvtsFloat (apvts, "changeRepeat"));
    o->setProperty ("pitch_jump_amount", readApvtsFloat (apvts, "changeAmount"));
    o->setProperty ("pitch_jump_onset_percent", readApvtsFloat (apvts, "changeSpeed"));
    o->setProperty ("pitch_jump_2_amount", readApvtsFloat (apvts, "changeAmount2"));
    o->setProperty ("pitch_jump_onset2_percent", readApvtsFloat (apvts, "changeSpeed2"));

    put ("squareDuty", "squareDuty");
    put ("dutySweep", "dutySweep");
    put ("repeatSpeed", "repeatSpeed");
    put ("flangerOffset", "flangerOffset");
    put ("flangerSweep", "flangerSweep");
    put ("lpFilterCutoff", "lpFilterCutoff");
    put ("lpFilterCutoffSweep", "lpFilterCutoffSweep");
    put ("lpFilterResonance", "lpFilterResonance");
    put ("hpFilterCutoff", "hpFilterCutoff");
    put ("hpFilterCutoffSweep", "hpFilterCutoffSweep");
    put ("bitCrush", "bitCrush");
    put ("bitCrushSweep", "bitCrushSweep");

    return juce::JSON::toString (juce::var (o.get()), false);
}
} // namespace bfxr_web
