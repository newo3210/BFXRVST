#pragma once

#include <JuceHeader.h>

class BfxrAbletonAudioProcessor;

/** Panel con sliders nativos (motor FigBug/bfxr) y barra de presets. */
class NativeParamsPanel : public juce::Component
{
public:
    explicit NativeParamsPanel (BfxrAbletonAudioProcessor&);
    void resized() override;

private:
    BfxrAbletonAudioProcessor& processorRef;

    juce::Viewport viewport;
    juce::Component paramsContainer;

    struct ParamRow
    {
        juce::Label name;
        juce::Slider slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    std::vector<std::unique_ptr<ParamRow>> paramRows;

    juce::TextButton btnRandom { "Randomize" };
    juce::TextButton btnMutate { "Mutate" };
    juce::TextButton btnPlay { "Play" };
    juce::TextButton btnCoin { "Coin" };
    juce::TextButton btnLaser { "Laser" };
    juce::TextButton btnExplosion { "Explosion" };
    juce::TextButton btnPowerup { "Powerup" };
    juce::TextButton btnHit { "Hit / hurt" };
    juce::TextButton btnJump { "Jump" };
    juce::TextButton btnBlip { "Blip / select" };

    juce::Component toolbar;

    void layoutParams();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NativeParamsPanel)
};
