#include "BfxrWebPanel.h"

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

BfxrWebPanel::BfxrWebPanel()
{
    auto opts = juce::WebBrowserComponent::Options();
#if JUCE_WINDOWS
    opts = opts.withBackend (juce::WebBrowserComponent::Options::Backend::webview2);
#endif
    browser = std::make_unique<juce::WebBrowserComponent> (std::move (opts));
    addAndMakeVisible (*browser);
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

void BfxrWebPanel::visibilityChanged()
{
    if (isShowing())
        tryLoadUrl();
}

void BfxrWebPanel::resized()
{
    if (browser != nullptr)
        browser->setBounds (getLocalBounds());
}
