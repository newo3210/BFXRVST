#pragma once

#include <JuceHeader.h>

/** Pestaña con la UI de bfxr2 (WebView2): copia local `bfxr2web` o https://www.bfxr.net */
class BfxrWebPanel : public juce::Component
{
public:
    BfxrWebPanel();
    ~BfxrWebPanel() override;
    void resized() override;
    void visibilityChanged() override;

private:
    std::unique_ptr<juce::WebBrowserComponent> browser;
    bool attemptedLoad = false;

    void tryLoadUrl();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BfxrWebPanel)
};
