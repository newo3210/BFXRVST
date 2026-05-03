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
    }

    return layout;
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
            if (auto* pi = dynamic_cast<juce::AudioParameterInt*> (param))
                out.setParam (p.uid, (float) pi->convertFrom0to1 (norm));
        }
        else if (auto* pf = dynamic_cast<juce::AudioParameterFloat*> (param))
        {
            out.setParam (p.uid, pf->convertFrom0to1 (norm));
        }
    }
}

void sfxrToApvts (const SfxrParams& in, juce::AudioProcessorValueTreeState& apvts)
{
    for (const auto& p : in.params)
    {
        auto* param = apvts.getParameter (p.uid);
        if (param == nullptr)
            continue;

        const float v = in.getParam (p.uid);

        if (p.uid == "waveType")
        {
            if (auto* pi = dynamic_cast<juce::AudioParameterInt*> (param))
                pi->setValueNotifyingHost (pi->convertTo0to1 ((int) std::lround (v)));
        }
        else if (auto* pf = dynamic_cast<juce::AudioParameterFloat*> (param))
        {
            pf->setValueNotifyingHost (pf->convertTo0to1 (v));
        }
    }
}
} // namespace bfxr
