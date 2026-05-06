#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <cmath>

BfxrAbletonAudioProcessor::BfxrAbletonAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", bfxr::createParameterLayout())
{
}

void BfxrAbletonAudioProcessor::prepareToPlay (double sampleRate, int)
{
    synth.setSampleRate ((float) sampleRate);

#if JUCE_DEBUG
    if (std::abs (sampleRate - 44100.0) > 1.0)
        DBG ("BfxrAbleton: SR del proyecto " << sampleRate
                                             << " Hz. El motor FigBug fue calibrado pensando en 44,1 kHz; ver docs/PARITY.md y docs/QA.md.");
#endif
}

bool BfxrAbletonAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& main = layouts.getMainOutputChannelSet();
    return main == juce::AudioChannelSet::mono() || main == juce::AudioChannelSet::stereo();
}

void BfxrAbletonAudioProcessor::armVoiceFromApvts()
{
    const juce::ScopedLock sl (synthMutex);
    SfxrParams p;
    bfxr::apvtsToSfxr (apvts, p);
    synth.setParams (p);
    synth.reset (true);
    voiceActive.store (true);
}

void BfxrAbletonAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn() && msg.getVelocity() > 0)
            armVoiceFromApvts();
    }

    const int numCh = buffer.getNumChannels();
    const int numSm = buffer.getNumSamples();

    buffer.clear();

    const juce::ScopedLock sl (synthMutex);

    for (int i = 0; i < numSm; ++i)
    {
        if (! voiceActive.load())
            continue;

        float tmp[1] { 0.0f };
        const bool ended = synth.synthWave (tmp, 0, 1);

        if (ended)
        {
            voiceActive.store (false);
            continue;
        }

        const float m = tmp[0];
        for (int ch = 0; ch < numCh; ++ch)
            buffer.getWritePointer (ch)[i] = m;
    }
}

juce::AudioProcessorEditor* BfxrAbletonAudioProcessor::createEditor()
{
    return new BfxrAbletonAudioProcessorEditor (*this);
}

void BfxrAbletonAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void BfxrAbletonAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

void BfxrAbletonAudioProcessor::applyRandomize()
{
    SfxrParams p;
    bfxr::prepareSfxrParamsFromApvts (apvts, p);
    p.randomize();
    bfxr::sfxrToApvts (p, apvts);
}

void BfxrAbletonAudioProcessor::applyMutate (float amount)
{
    SfxrParams p;
    bfxr::prepareSfxrParamsFromApvts (apvts, p);
    p.mutate (amount);
    bfxr::sfxrToApvts (p, apvts);
}

void BfxrAbletonAudioProcessor::applyPickupCoin()
{
    SfxrParams p;
    bfxr::apvtsToSfxr (apvts, p);
    p.generatePickupCoin();
    bfxr::sfxrToApvts (p, apvts);
}

void BfxrAbletonAudioProcessor::applyLaserShoot()
{
    SfxrParams p;
    bfxr::apvtsToSfxr (apvts, p);
    p.generateLaserShoot();
    bfxr::sfxrToApvts (p, apvts);
}

void BfxrAbletonAudioProcessor::applyExplosion()
{
    SfxrParams p;
    bfxr::apvtsToSfxr (apvts, p);
    p.generateExplosion();
    bfxr::sfxrToApvts (p, apvts);
}

void BfxrAbletonAudioProcessor::applyPowerup()
{
    SfxrParams p;
    bfxr::apvtsToSfxr (apvts, p);
    p.generatePowerup();
    bfxr::sfxrToApvts (p, apvts);
}

void BfxrAbletonAudioProcessor::applyHitHurt()
{
    SfxrParams p;
    bfxr::apvtsToSfxr (apvts, p);
    p.generateHitHurt();
    bfxr::sfxrToApvts (p, apvts);
}

void BfxrAbletonAudioProcessor::applyJump()
{
    SfxrParams p;
    bfxr::apvtsToSfxr (apvts, p);
    p.generateJump();
    bfxr::sfxrToApvts (p, apvts);
}

void BfxrAbletonAudioProcessor::applyBlipSelect()
{
    SfxrParams p;
    bfxr::apvtsToSfxr (apvts, p);
    p.generateBlipSelect();
    bfxr::sfxrToApvts (p, apvts);
}

void BfxrAbletonAudioProcessor::triggerPreview()
{
    armVoiceFromApvts();
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BfxrAbletonAudioProcessor();
}
