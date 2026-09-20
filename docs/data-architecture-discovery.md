# Data Architecture Discovery (BFXRVST)

> Access-pattern gate before inventing persistence. This plugin is **not** a multi-tenant web app with a database.

**Last reviewed:** 2026-09-20

## Current persistence reality

| Store | What | Where |
|-------|------|--------|
| Host project state | APVTS ValueTree XML (params, locks, `rePreview`) | DAW project via `getStateInformation` / `setStateInformation` |
| Packaged web assets | Static bfxr2 files | `bfxr2web/` next to VST3 binary (post-build copy) |
| Source submodules | FigBug/bfxr, increpare/bfxr2 | `third_party/` |
| Optional future | jsfxr Serialize JSON / Base58 presets | User paste/import (no server) |

**There is no server-side DB, no Prisma/Postgres, no auth store.**

## Access Pattern Matrix (plugin)

| Feature / Query | Frequency | Ops | Consistency | Strategy |
|---|---|---|---|---|
| Load/save Live set | Med | R/W | Strong (must restore knobs) | APVTS XML blob |
| MIDI / Play one-shot | High | R params → W audio | Strong within shot | Snapshot params at arm |
| Randomize / Mutate | Med | W many params | Strong UI update | APVTS notify host |
| Web → motor import | Low–Med | R JSON → W APVTS | Strong after button | Explicit map + clamp |
| Motor → web export | Low–Med | R APVTS → W JS | Best-effort UI | Explicit map + DBG on fail |
| Automation from Live | High | W single params | Host-owned | APVTS parameters |

## STOP-and-ask gate

Before adding any new persistence (files on disk, cloud sync, preset library DB, telemetry):

1. Fill a new row in the matrix above.
2. State consistency needs (crash safety, multi-instance).
3. Get human OK in the OpenSpec `design.md`.

Do **not** invent a backend database for this VST unless the product idea explicitly changes.

## Selected approach (today)

- **In-host state only** via JUCE APVTS.
- **Trade-off:** simple and Ableton-native; no cross-DAW cloud library unless built later.
- **Indexing:** N/A.
