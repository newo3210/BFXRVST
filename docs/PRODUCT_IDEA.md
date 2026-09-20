# Product idea — BFXRVST (Bfxr Ableton)

**BFXRVST** is a **Bfxr-complete VST3** instrument for **Ableton Live on Windows**. It is **not** a downgrade to classic sfxr: the motor and native UI stay aligned with **FigBug/bfxr**, with an embedded **bfxr2** web panel for the modern Bfxr workflow.

Last updated: 2026-09-20

---

## What we ship

| Pillar | Choice |
|--------|--------|
| Format / OS | **VST3**, **Windows** (Ableton-first) |
| Synth motor | **FigBug/bfxr** (`SfxrParams` / `SfxrSynth`) via JUCE 8 |
| Web UI | Embedded **bfxr2** (WebView2), local `bfxr2web` copy + bridges |
| Explicitly out | **No Footsteppr** (not registered in the embedded bfxr2 patch) |
| Completeness | Full Bfxr param set (wave types 0–8, compression, overtones, dual pitch jump, bit crush, …) |

The native tab exposes all `SfxrParams` uids, classic-style presets, Randomize/Mutate with per-param locks, MIDI / Play triggering, and project state via APVTS XML.

---

## Role of sfxr.me / jsfxr

[sfxr.me](https://sfxr.me) ([jsfxr](https://github.com/chr15m/jsfxr)) is a useful **external reference**, not the product target:

- Documents the **classic sfxr subset** (~23 `p_*` fields, `wave_type` 0–3, volume + sample export).
- Provides **JSON** (`oldParams: true`) and **Base58** interchange familiar to game developers.
- Helps explain mapping and gaps vs our richer Bfxr set (`docs/SFXR_ME_PARAMS.md`, `docs/PARAM_GAP_ANALYSIS.md`).

Using sfxr.me as a reference must **never** imply shrinking BFXRVST down to classic-only parameters.

---

## Future optional: jsfxr import

A possible later feature:

- Import **jsfxr Serialize JSON** and/or **Base58** into the plugin **APVTS**.
- Map **classic** fields to FigBug uids (`p_env_attack` → `attackTime`, etc.).
- Leave **Bfxr-only** parameters at their defaults (compression, overtones, second pitch jump, bit crush, wave types 4–8, …).

This is an **interoperability** nicety for the game-dev / sfxr ecosystem. Primary parity remains **FigBug/bfxr + embedded bfxr2**.

---

## Non-goals (for this product line)

- Replacing the motor with pure classic sfxr / jsfxr DSP.
- Making sfxr.me the primary UI.
- Shipping Footsteppr inside the VST package.
- Claiming 1:1 visual parity with bfxr2.net layout (bridges + “same family” sound are the bar; see `docs/PARITY.md`).

---

## Related docs

| Doc | Purpose |
|-----|---------|
| `docs/PARITY.md` | Web ↔ motor parity, bridges, SR notes |
| `docs/SFXR_ME_PARAMS.md` | Full jsfxr / sfxr.me Params catalog |
| `docs/PARAM_GAP_ANALYSIS.md` | Classic ↔ Bfxr field mapping and gaps |
| `docs/QA.md` | Pre-release checklist |
| `docs/RELEASE.md` | Build, CI, WebView2 / MSVC notes |
