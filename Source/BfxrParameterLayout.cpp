#include "BfxrParameterLayout.h"

#include <cmath>

#include "SfxrParams.h"

namespace bfxr
{
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    SfxrParams prototype;

    for (const auto& p : prototype.params)
    {
        const juce::ParameterID pid { p.uid, 1 };

        if (p.uid == "waveType")
        {
            const int maxIdx = SfxrParams::WAVETYPECOUNT - 1;
            layout.add (std::make_unique<juce::AudioParameterInt> (pid, p.name, 0, maxIdx, (int) std::lround (p.defaultValue)));
        }
        else
        {
            juce::NormalisableRange<float> range { p.minValue, p.maxValue };
            layout.add (std::make_unique<juce::AudioParameterFloat> (pid, p.name, range, p.defaultValue,
                                                                     juce::AudioParameterFloatAttributes().withLabel ("")));
        }

        if (p.uid != "masterVolume")
        {
            const juce::String lockId = "lock_" + juce::String (p.uid);
            layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { lockId, 1 },
                                                                     "Lock " + p.name,
                                                                     false));
        }
    }

    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { "rePreview", 1 },
                                                             "Re-preview al soltar slider",
                                                             false));

    return layout;
}

static void syncLocksFromApvtsToSfxr (const juce::AudioProcessorValueTreeState& apvts, SfxrParams& p)
{
    p.setAllLocked (false);
    p.setParamLocked ("masterVolume", true);

    SfxrParams prototype;
    for (const auto& par : prototype.params)
    {
        if (par.uid == "masterVolume")
            continue;

        const juce::String lockId = "lock_" + juce::String (par.uid);
        if (auto* raw = apvts.getRawParameterValue (lockId))
            p.setParamLocked (par.uid, raw->load() > 0.5f);
    }
}

void prepareSfxrParamsFromApvts (const juce::AudioProcessorValueTreeState& apvts, SfxrParams& out)
{
    apvtsToSfxr (apvts, out);
    syncLocksFromApvtsToSfxr (apvts, out);
}

void apvtsToSfxr (const juce::AudioProcessorValueTreeState& apvts, SfxrParams& out)
{
    for (const auto& p : out.params)
    {
        auto* param = apvts.getParameter (p.uid);
        if (param == nullptr)
            continue;

        const float norm = apvts.getRawParameterValue (p.uid)->load();

        if (p.uid == "waveType")
        {
            if (auto* pInt = dynamic_cast<juce::AudioParameterInt*> (param))
                out.setParam (p.uid, (float) pInt->convertFrom0to1 (norm));
        }
        else if (auto* pf = dynamic_cast<juce::AudioParameterFloat*> (param))
        {
            out.setParam (p.uid, pf->convertFrom0to1 (norm));
        }
    }
}

void sfxrToApvts (const SfxrParams& in, juce::AudioProcessorValueTreeState& apvts)
{
    SfxrParams copy = in;

    for (const auto& p : copy.params)
    {
        auto* param = apvts.getParameter (p.uid);
        if (param == nullptr)
            continue;

        const float v = copy.getParam (p.uid);

        if (p.uid == "waveType")
        {
            if (auto* pInt = dynamic_cast<juce::AudioParameterInt*> (param))
                pInt->setValueNotifyingHost (pInt->convertTo0to1 ((float) std::lround (v)));
        }
        else if (auto* pf = dynamic_cast<juce::AudioParameterFloat*> (param))
        {
            pf->setValueNotifyingHost (pf->convertTo0to1 (v));
        }
    }
}
} // namespace bfxr
