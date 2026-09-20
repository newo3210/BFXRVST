# UI / editor standards (BFXRVST)

> Formerly named `frontend-standards.md` in the OpenSpecs template. This project UI is **JUCE native + WebView2**, not React.

## Stack

| Piece | Choice |
|-------|--------|
| Native UI | JUCE `AudioProcessorEditor`, tabs, sliders, buttons |
| Web UI | Embedded bfxr2 (`bfxr2web` beside plugin binary) via WebView2 |
| Bridge | Explicit import/export buttons (no auto-sync MVP) |

## Rules

1. **Thin presentation:** panels layout and wire callbacks; business/mapping lives in bridge helpers / processor.
2. **Two tabs:** Native (`NativeParamsPanel`) and bfxr2 web (`BfxrWebPanel`). Keep responsibilities separate.
3. **Locks UI:** expose per-parameter lock toggles (except master volume, forced locked in motor).
4. **Re-preview:** optional; trigger preview on slider *release*, not continuous drag (unless a future change specifies otherwise).
5. **Web content:** prefer local `bfxr2web`; fallback remote only as documented. Footsteppr must stay patched out of the packaged copy.
6. **Bridge UX:** require active **Bfxr** web tab for import/export; surface failures via Debug logging, not host crashes.
7. **Accessibility / resize:** editor must remain usable when resized (`docs/QA.md`).
8. **Do not** embed synthesis loops or raw `SfxrSynth` calls from UI without processor APIs.

## Entry points

- `Source/PluginEditor.*`
- `Source/NativeParamsPanel.*`
- `Source/BfxrWebPanel.*`
