# Plugin / audio engine standards (BFXRVST)

> Formerly named `backend-standards.md` in the OpenSpecs template. This project has no HTTP API; this file covers the **native audio + parameter** side.

## Stack

| Piece | Choice |
|-------|--------|
| Language | C++17 |
| Framework | JUCE 8.0.4 (FetchContent) |
| Format | VST3 instrument, Windows only |
| Synth motor | FigBug/bfxr (`SfxrParams`, `SfxrSynth`) in `third_party/bfxr` |
| Build | CMake ≥ 3.22, MSVC (VS 2022), GitHub Actions Windows |

## Rules

1. **Realtime safety:** `processBlock` must not allocate, lock UI-only resources, or call into WebView/JS. Keep synthesis under `synthMutex` (or an equivalent realtime-safe scheme if upgraded).
2. **Single voice model (current):** one-shot from MIDI note-on or `triggerPreview`; parameters freeze at arm time unless a documented live-update feature is specified.
3. **APVTS is source of truth for host automation / project save.** Sync to `SfxrParams` at arm / preset apply; sync locks via `lock_<uid>` bools.
4. **Do not put DSP formulas in Editor panels.** Call processor methods.
5. **Sample rate:** call `SfxrSynth::setSampleRate` from `prepareToPlay`. Document parity expectations in `docs/PARITY.md` (44.1 kHz reference vs 48 kHz host).
6. **Third-party:** minimize edits under `third_party/`; prefer adapter code in `Source/`.
7. **WebView2:** compile/link via NuGet package path (`JUCE_WEBVIEW2_PACKAGE_LOCATION`); see `docs/RELEASE.md`.
8. **Testing:** prefer unit tests for pure mapping (bridge JSON ↔ APVTS) when added; until then, execute `docs/QA.md` scenarios for release claims.

## Entry points

- `Source/PluginProcessor.cpp` — audio + MIDI + state
- `Source/BfxrParameterLayout.cpp` — APVTS layout + Sfxr sync
- `Source/BfxrWebBridge.cpp` — bfxr2 JSON mapping
