#pragma once

#include <JuceHeader.h>

class BfxrAbletonAudioProcessor;

/** Pestaña con bfxr2 en WebView2 y acciones hacia el motor nativo. */
class BfxrWebPanel : public juce::Component
{
public:
    explicit BfxrWebPanel (BfxrAbletonAudioProcessor&);
    ~BfxrWebPanel() override;
    void resized() override;
    void visibilityChanged() override;

private:
    BfxrAbletonAudioProcessor& processor;

    std::unique_ptr<juce::WebBrowserComponent> browser;
    juce::TextButton btnImport { "Importar web -> motor VST" };
    juce::TextButton btnPlayVst { "Play motor VST" };

    bool attemptedLoad = false;
    bool bridgeInjected = false;

    void tryLoadUrl();
    void injectBridge();
    void pullParamsFromWeb();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BfxrWebPanel)
};
