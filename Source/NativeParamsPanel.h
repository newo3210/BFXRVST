#pragma once

#include <JuceHeader.h>

class BfxrAbletonAudioProcessor;

/** Panel con sliders nativos (motor FigBug/bfxr) y barra de presets. */
class NativeParamsPanel : public juce::Component,
                          private juce::Slider::Listener
{
public:
    explicit NativeParamsPanel (BfxrAbletonAudioProcessor&);
    ~NativeParamsPanel() override;
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
        juce::ToggleButton lockBtn;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lockAttachment;
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

    juce::ToggleButton rePreview { "Re-preview al soltar slider" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> rePreviewAttachment;

    juce::Component toolbar;

    void layoutParams();
    void sliderValueChanged (juce::Slider*) override {}
    void sliderDragEnded (juce::Slider*) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NativeParamsPanel)
};
