# Architecture SDD

> **Language:** English (technical contract).  
> **Location:** repository root `ARCHITECTURE_SDD.md`.

**Last updated:** 2026-09-20  
**Related OpenSpec change:** `openspec/` bootstrap (legacy SDD init; no feature change yet)

---

## 1. System overview

**BFXRVST** (“Bfxr Ableton”) is a **Windows VST3 instrument** that generates retro game-style one-shot SFX inside Ableton Live. Users edit parameters on a **native JUCE panel** and/or an embedded **bfxr2** WebView, optionally bridge JSON both ways, and trigger sound via MIDI or Play. The DSP core is **FigBug/bfxr**; the product target is **Bfxr-complete**, not classic-sfxr-only (see `docs/PRODUCT_IDEA.md`).

Primary journeys:

1. Load plugin → tweak native sliders / presets → MIDI or Play → hear one-shot.
2. Open bfxr2 tab → Import web → motor / Send motor → web → continue editing.
3. Save Live set → reopen → APVTS state (including locks) restored.

---

## 2. Layer mapping (this project)

| Conceptual layer | Paths in this repo | Notes |
|---|---|---|
| Presentation | `Source/PluginEditor.*`, `NativeParamsPanel.*`, `BfxrWebPanel.*` | Tabs, controls, WebView host |
| Application / services | `Source/BfxrParameterLayout.*`, `BfxrWebBridge.*`, preset methods on `PluginProcessor` | Mapping, locks, bridge, randomize/mutate |
| Infrastructure / audio | `Source/PluginProcessor.*` (`processBlock`), `third_party/bfxr/*`, `cmake/`, `.github/workflows/` | Realtime synth, packaging, CI |
| Contracts | `SfxrParams` uids/ranges; bridge JSON maps; docs `PARAM_GAP_ANALYSIS.md` | No Zod; C++ + documented JSON |

---

## 3. Data & control flow

```text
Ableton Live (MIDI / automation / project XML)
  → PluginProcessor (APVTS + processBlock)
      → SfxrSynth (third_party/bfxr) → audio out

Editor (NativeParamsPanel / BfxrWebPanel)
  → APVTS / BfxrWebBridge
      → PluginProcessor trigger / preset helpers
```

Sequence notes:

- **Arm:** note-on or Play copies APVTS → `SfxrParams` → `synth.reset` under mutex.
- **Web import:** JS eval → JSON string → `applyBfxr2JsonToApvts`.
- **Web export:** `apvtsToBfxr2JsonString` → inject into page helpers.
- **Errors:** bad JS/bridge → `DBG` in Debug; avoid throwing across host boundary.

---

## 4. API routes

N/A — no HTTP server. Host APIs are VST3 / JUCE (`AudioProcessor`, APVTS, WebView2).

---

## 5. Data schemas & models

### 5.0 Access Pattern Analysis

See `docs/data-architecture-discovery.md`. Selected approach: **APVTS XML in the DAW project**; no DB.

### 5.1 Contracts

- **APVTS parameters:** one float/int per `SfxrParams` uid + `lock_<uid>` bools + `rePreview`.
- **bfxr2 JSON:** field aliases documented in `BfxrWebBridge.cpp` / `docs/PARITY.md`.
- **jsfxr / sfxr.me (reference only today):** `oldParams` JSON catalogued in `docs/SFXR_ME_PARAMS.md`; gap table in `docs/PARAM_GAP_ANALYSIS.md`.

### 5.2 Persistence models

Host `MemoryBlock` XML from `apvts.copyState()`.

---

## 6. AI boundaries

None in the current product.

---

## 7. Error handling

- Clamp via `SfxrParams::setParam`.
- Mutex around synth set/process.
- WebView blank / missing `bfxr2web`: documented in `docs/RELEASE.md` / `docs/QA.md`.

---

## 8. Gaps / follow-ups

- Optional jsfxr JSON/Base58 import (product idea).
- OpenSpec feature changes not yet created (bootstrap only).
- Automated unit tests for bridge mapping not yet present.
