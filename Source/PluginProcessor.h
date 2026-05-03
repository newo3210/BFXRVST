#pragma once

#include <JuceHeader.h>

#include "BfxrParameterLayout.h"
#include "SfxrSynth.h"

class BfxrAbletonAudioProcessor : public juce::AudioProcessor
{
public:
    BfxrAbletonAudioProcessor();
    ~BfxrAbletonAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    void applyRandomize();
    void applyMutate (float amount = 0.05f);
    void applyPickupCoin();
    void applyLaserShoot();
    void applyExplosion();
    void applyPowerup();
    void applyHitHurt();
    void applyJump();
    void applyBlipSelect();

    /** Dispara un one-shot con los parámetros actuales (hilo de mensajes). */
    void triggerPreview();

private:
    SfxrSynth synth { 44100.0f };
    std::atomic<bool> voiceActive { false };
    juce::CriticalSection synthMutex;

    void armVoiceFromApvts();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BfxrAbletonAudioProcessor)
};
