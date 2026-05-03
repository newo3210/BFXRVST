#include "PluginEditor.h"

namespace
{
constexpr int kEditorMinW = 960;
constexpr int kEditorMinH = 640;
} // namespace

BfxrAbletonAudioProcessorEditor::BfxrAbletonAudioProcessorEditor (BfxrAbletonAudioProcessor& p)
    : AudioProcessorEditor (&p),
      nativePanel (p),
      webPanel (p)
{
    setSize (kEditorMinW, kEditorMinH);
    setResizable (true, true);
    setResizeLimits (kEditorMinW, kEditorMinH, 2000, 1200);

    tabs.addTab ("Nativo (VST)", juce::Colours::darkgrey, &nativePanel, false);
    tabs.addTab ("bfxr2 (web)", juce::Colours::darkgrey, &webPanel, false);
    addAndMakeVisible (tabs);
}

void BfxrAbletonAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void BfxrAbletonAudioProcessorEditor::resized()
{
    tabs.setBounds (getLocalBounds().reduced (8));
}
