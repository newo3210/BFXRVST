#include "NativeParamsPanel.h"

#include "PluginProcessor.h"
#include "SfxrParams.h"

namespace
{
constexpr int kToolbarH = 90;
constexpr int kSliderH = 120;
constexpr int kLabelH = 22;
constexpr int kRowH = kLabelH + kSliderH + 8;
constexpr int kLockW = 22;
} // namespace

NativeParamsPanel::NativeParamsPanel (BfxrAbletonAudioProcessor& p)
    : processorRef (p)
{
    addAndMakeVisible (toolbar);
    toolbar.addAndMakeVisible (btnRandom);
    toolbar.addAndMakeVisible (btnMutate);
    toolbar.addAndMakeVisible (btnPlay);
    toolbar.addAndMakeVisible (btnCoin);
    toolbar.addAndMakeVisible (btnLaser);
    toolbar.addAndMakeVisible (btnExplosion);
    toolbar.addAndMakeVisible (btnPowerup);
    toolbar.addAndMakeVisible (btnHit);
    toolbar.addAndMakeVisible (btnJump);
    toolbar.addAndMakeVisible (btnBlip);
    toolbar.addAndMakeVisible (rePreview);

    rePreviewAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "rePreview", rePreview);

    btnRandom.onClick = [this] { processorRef.applyRandomize(); };
    btnMutate.onClick = [this] { processorRef.applyMutate (0.05f); };
    btnPlay.onClick = [this] { processorRef.triggerPreview(); };
    btnCoin.onClick = [this] { processorRef.applyPickupCoin(); };
    btnLaser.onClick = [this] { processorRef.applyLaserShoot(); };
    btnExplosion.onClick = [this] { processorRef.applyExplosion(); };
    btnPowerup.onClick = [this] { processorRef.applyPowerup(); };
    btnHit.onClick = [this] { processorRef.applyHitHurt(); };
    btnJump.onClick = [this] { processorRef.applyJump(); };
    btnBlip.onClick = [this] { processorRef.applyBlipSelect(); };

    SfxrParams prototype;
    for (const auto& spec : prototype.params)
    {
        auto row = std::make_unique<ParamRow>();
        row->name.setText (spec.name, juce::dontSendNotification);
        row->name.setJustificationType (juce::Justification::centredLeft);
        row->name.setMinimumHorizontalScale (0.75f);

        row->slider.setSliderStyle (juce::Slider::LinearVertical);
        row->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 72, 20);
        row->slider.setRange (0.0, 1.0, 0.0);

        row->attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            processorRef.apvts, spec.uid, row->slider);

        row->slider.addListener (this);

        if (spec.uid != "masterVolume")
        {
            row->lockBtn.setButtonText ("L");
            row->lockBtn.setTooltip ("Bloquear en Randomize/Mutate");
            row->lockAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                processorRef.apvts, "lock_" + juce::String (spec.uid), row->lockBtn);
            paramsContainer.addAndMakeVisible (row->lockBtn);
        }

        paramsContainer.addAndMakeVisible (row->name);
        paramsContainer.addAndMakeVisible (row->slider);
        paramRows.push_back (std::move (row));
    }

    viewport.setViewedComponent (&paramsContainer, false);
    addAndMakeVisible (viewport);
    layoutParams();
}

NativeParamsPanel::~NativeParamsPanel()
{
    for (auto& row : paramRows)
        row->slider.removeListener (this);
}

void NativeParamsPanel::sliderDragEnded (juce::Slider*)
{
    if (processorRef.apvts.getRawParameterValue ("rePreview") == nullptr)
        return;

    if (processorRef.apvts.getRawParameterValue ("rePreview")->load() < 0.5f)
        return;

    processorRef.triggerPreview();
}

void NativeParamsPanel::layoutParams()
{
    const int cols = 8;
    const int colW = 96;
    const int pad = 8;
    int maxRow = 0;

    for (size_t i = 0; i < paramRows.size(); ++i)
    {
        const int c = (int) i % cols;
        const int r = (int) i / cols;
        maxRow = juce::jmax (maxRow, r);
        const int x = pad + c * colW;
        const int y = pad + r * kRowH;

        auto& row = *paramRows[i];
        row.name.setBounds (x, y, colW - 4, kLabelH);
        const int sliderW = colW - (row.lockAttachment != nullptr ? (kLockW + 6) : 4);
        row.slider.setBounds (x, y + kLabelH, sliderW, kSliderH);

        if (row.lockAttachment != nullptr)
            row.lockBtn.setBounds (x + colW - kLockW - 4, y + kLabelH + kSliderH / 2 - kLockW / 2, kLockW, kLockW);
    }

    const int contentH = pad * 2 + (maxRow + 1) * kRowH;
    const int contentW = pad * 2 + cols * colW;
    paramsContainer.setSize (contentW, contentH);
}

void NativeParamsPanel::resized()
{
    auto r = getLocalBounds();
    toolbar.setBounds (r.removeFromTop (kToolbarH));

    const int gap = 6;
    auto area = toolbar.getLocalBounds().reduced (4);
    auto row1 = area.removeFromTop (40);
    auto placeBtn = [&] (juce::Button& b, int w) {
        b.setBounds (row1.removeFromLeft (w).reduced (0, 2));
        row1.removeFromLeft (gap);
    };
    placeBtn (btnRandom, 88);
    placeBtn (btnMutate, 72);
    placeBtn (btnPlay, 56);
    row1.removeFromLeft (gap);
    placeBtn (btnCoin, 56);
    placeBtn (btnLaser, 56);
    placeBtn (btnExplosion, 76);
    placeBtn (btnPowerup, 72);
    placeBtn (btnHit, 88);
    placeBtn (btnJump, 56);
    placeBtn (btnBlip, 96);

    area.removeFromTop (4);
    rePreview.setBounds (area.removeFromTop (36).withWidth (280));

    r.removeFromTop (4);
    viewport.setBounds (r);
    layoutParams();
}
