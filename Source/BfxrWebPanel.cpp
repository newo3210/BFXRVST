#include "BfxrWebPanel.h"

#include "BfxrWebBridge.h"
#include "PluginProcessor.h"

namespace
{
juce::File resolveBundledBfxr2Root()
{
    const auto exe = juce::File::getSpecialLocation (juce::File::currentExecutableFile);
    const auto bundled = exe.getParentDirectory().getChildFile ("bfxr2web");

    if (bundled.getChildFile ("index.html").existsAsFile())
        return bundled;

#if JUCE_DEBUG
    const auto devTree = juce::File (__FILE__).getParentDirectory().getParentDirectory().getChildFile ("third_party").getChildFile ("bfxr2");
    if (devTree.getChildFile ("index.html").existsAsFile())
        return devTree;
#endif

    return {};
}
} // namespace

BfxrWebPanel::BfxrWebPanel (BfxrAbletonAudioProcessor& p)
    : processor (p)
{
    auto opts = juce::WebBrowserComponent::Options();
#if JUCE_WINDOWS
    opts = opts.withBackend (juce::WebBrowserComponent::Options::Backend::webview2);
#endif
    browser = std::make_unique<juce::WebBrowserComponent> (std::move (opts));
    addAndMakeVisible (*browser);

    addAndMakeVisible (btnImport);
    addAndMakeVisible (btnPlayVst);

    btnImport.onClick = [this] { pullParamsFromWeb(); };
    btnPlayVst.onClick = [this] { processor.triggerPreview(); };
}

BfxrWebPanel::~BfxrWebPanel() = default;

void BfxrWebPanel::tryLoadUrl()
{
    if (attemptedLoad || browser == nullptr)
        return;

    attemptedLoad = true;

    const auto root = resolveBundledBfxr2Root();
    if (root.exists())
    {
        const juce::URL url (root.getChildFile ("index.html"));
        browser->goToURL (url.toString (true));
    }
    else
    {
        browser->goToURL ("https://www.bfxr.net/");
    }
}

void BfxrWebPanel::injectBridge()
{
    if (bridgeInjected || browser == nullptr)
        return;

    bridgeInjected = true;
    browser->evaluateJavascript (bfxr_web::getBridgeInjectionScript(), nullptr);
}

void BfxrWebPanel::pullParamsFromWeb()
{
    if (browser == nullptr)
        return;

    injectBridge();

    const juce::String script = R"JS(
(function(){
  try {
    if (typeof window.__bfxrVstGetActiveSynthParams !== 'function')
      return '{"error":"no_bridge"}';
    var p = window.__bfxrVstGetActiveSynthParams();
    if (!p) return '{"error":"no_bfxr_tab"}';
    return JSON.stringify(p);
  } catch (e) {
    return JSON.stringify({error:String(e)});
  }
})()
)JS";

    browser->evaluateJavascript (script, [this] (const juce::WebBrowserComponent::EvaluationResult& eval) {
        if (eval.getError() != nullptr)
            return;

        const auto* res = eval.getResult();
        if (res == nullptr || ! res->isString())
            return;

        const juce::String s = res->toString();
        if (! s.startsWith ("{"))
            return;

        bfxr_web::applyBfxr2JsonToApvts (s, processor.apvts);
    });
}

void BfxrWebPanel::visibilityChanged()
{
    if (isShowing())
        tryLoadUrl();
}

void BfxrWebPanel::resized()
{
    auto r = getLocalBounds();
    auto top = r.removeFromTop (36);
    btnImport.setBounds (top.removeFromLeft (220).reduced (4, 4));
    btnPlayVst.setBounds (top.removeFromLeft (140).reduced (4, 4));

    if (browser != nullptr)
        browser->setBounds (r);
}
