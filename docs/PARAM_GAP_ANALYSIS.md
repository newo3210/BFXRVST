# Parameter gap analysis: jsfxr / sfxr.me ↔ FigBug/bfxr (BFXRVST)

**Goal:** Show how classic sfxr (jsfxr / [sfxr.me](https://sfxr.me)) relates to the **FigBug/bfxr** `SfxrParams` set used by **BFXRVST**.

**Headline:** Classic sfxr is a **subset** of our Bfxr parameter set. BFXRVST stays Bfxr-complete; sfxr.me is a reference for the classic subset and for JSON/Base58 interchange with the game-dev ecosystem.

Sources:

- jsfxr: `Params` in [chr15m/jsfxr `sfxr.js`](https://github.com/chr15m/jsfxr/blob/master/sfxr.js)
- Bfxr motor: `third_party/bfxr/SfxrParams.h` (`params` vector uids)

Last updated: 2026-09-20

---

## Status legend

| Status | Meaning |
|--------|---------|
| **equivalent** | Same role; map 1:1 (clamp to Bfxr ranges) |
| **subset** | jsfxr value space is smaller (e.g. `wave_type` 0–3 vs Bfxr 0–8) |
| **missing on jsfxr** | Present in FigBug/bfxr / BFXRVST; no classic sfxr field |
| **missing on Bfxr** | Present in jsfxr JSON/export; no `SfxrParams` uid (host/export concern) |

---

## Mapping table (classic → Bfxr)

| jsfxr field | FigBug / BFXRVST uid | Status | Notes |
|-------------|----------------------|--------|-------|
| `wave_type` | `waveType` | **subset** | jsfxr **0–3**; Bfxr **0–8** (adds triangle, pink, tan, whistle, breaker). Map 0–3 as-is; leave 4–8 unused on import. |
| `p_env_attack` | `attackTime` | **equivalent** | \[0, 1\] |
| `p_env_sustain` | `sustainTime` | **equivalent** | \[0, 1\] |
| `p_env_punch` | `sustainPunch` | **equivalent** | \[0, 1\] |
| `p_env_decay` | `decayTime` | **equivalent** | \[0, 1\] |
| `p_base_freq` | `startFrequency` | **equivalent** | \[0, 1\] |
| `p_freq_limit` | `minFrequency` | **equivalent** | \[0, 1\] |
| `p_freq_ramp` | `slide` | **equivalent** | SIGNED \[−1, 1\] |
| `p_freq_dramp` | `deltaSlide` | **equivalent** | SIGNED \[−1, 1\] |
| `p_vib_strength` | `vibratoDepth` | **equivalent** | \[0, 1\] |
| `p_vib_speed` | `vibratoSpeed` | **equivalent** | \[0, 1\] |
| `p_arp_mod` | `changeAmount` | **equivalent** | SIGNED \[−1, 1\] (first pitch jump) |
| `p_arp_speed` | `changeSpeed` | **equivalent** | \[0, 1\] |
| `p_duty` | `squareDuty` | **equivalent** | \[0, 1\] |
| `p_duty_ramp` | `dutySweep` | **equivalent** | SIGNED \[−1, 1\] |
| `p_repeat_speed` | `repeatSpeed` | **equivalent** | \[0, 1\] |
| `p_pha_offset` | `flangerOffset` | **equivalent** | SIGNED \[−1, 1\] |
| `p_pha_ramp` | `flangerSweep` | **equivalent** | SIGNED \[−1, 1\] |
| `p_lpf_freq` | `lpFilterCutoff` | **equivalent** | \[0, 1\] |
| `p_lpf_ramp` | `lpFilterCutoffSweep` | **equivalent** | SIGNED \[−1, 1\] |
| `p_lpf_resonance` | `lpFilterResonance` | **equivalent** | \[0, 1\] |
| `p_hpf_freq` | `hpFilterCutoff` | **equivalent** | \[0, 1\] |
| `p_hpf_ramp` | `hpFilterCutoffSweep` | **equivalent** | SIGNED \[−1, 1\] |
| `sound_vol` | `masterVolume` | **equivalent** | \[0, 1\] UI; synth gain curves may differ slightly |
| `sample_rate` | — | **missing on Bfxr** | Export/UI only in jsfxr; host sets SR in the VST |
| `sample_size` | — | **missing on Bfxr** | WAV bit depth in jsfxr; N/A to realtime APVTS |
| `oldParams` | — | **missing on Bfxr** | Format flag for jsfxr JSON only |

---

## Bfxr-only parameters (missing on jsfxr)

These exist on FigBug/bfxr and BFXRVST. On a future **jsfxr → APVTS** import, leave them at **SfxrParams defaults**.

| Bfxr uid | Default (FigBug) | Range | Role |
|----------|------------------|-------|------|
| `waveType` values **4–8** | (type default `2`) | 0–8 | Triangle, pink noise, tan, whistle, breaker |
| `compressionAmount` | `0.3` | \[0, 1\] | Amplitude compression / punch against music beds |
| `overtones` | `0` | \[0, 1\] | Harmonic overlays (copies at multiples of frequency) |
| `overtoneFalloff` | `0` | \[0, 1\] | How fast higher overtones decay |
| `changeRepeat` | `0` | \[0, 1\] | Pitch-jump repeat / arpeggiation density |
| `changeAmount2` | `0` | \[−1, 1\] | Second pitch jump amount |
| `changeSpeed2` | `0` | \[0, 1\] | Second pitch jump onset |
| `bitCrush` | `0` | \[0, 1\] | Resample / crush frequency |
| `bitCrushSweep` | `0` | \[−1, 1\] | Bit-crush sweep |

---

## Wave type detail

| Index | jsfxr / classic sfxr | FigBug/bfxr |
|------:|----------------------|-------------|
| 0 | Square | Square |
| 1 | Sawtooth | Saw |
| 2 | Sine | Sin |
| 3 | Noise | Noise |
| 4 | — | Triangle |
| 5 | — | Pink noise |
| 6 | — | Tan |
| 7 | — | Whistle |
| 8 | — | Breaker |

Status for the enum as a whole: **subset** on the jsfxr side.

---

## Summary counts

| Bucket | Count (approx.) |
|--------|----------------:|
| Classic fields with **equivalent** Bfxr uid | 23 (`wave_type` counted under subset) + volume |
| **subset** | `wave_type` → `waveType` |
| **missing on jsfxr** (Bfxr extras) | compression, overtones×2, dual jump×3, bitCrush×2, plus wave indices 4–8 |
| **missing on Bfxr** | `sample_rate`, `sample_size`, `oldParams` (and optional legacy `p_vib_delay` if ever seen) |

---

## Import sketch (future optional)

If BFXRVST adds **Import jsfxr Serialize JSON / Base58**:

1. Detect `oldParams: true` or Base58 classic payload.
2. Map classic fields per the table above into APVTS / `SfxrParams`.
3. Leave all **missing on jsfxr** Bfxr params at defaults.
4. Ignore or apply outside the synth: `sample_rate` / `sample_size` (host / export only).
5. Do **not** strip Bfxr-only UI; product remains Bfxr-complete.

See also: `docs/SFXR_ME_PARAMS.md`, `docs/PRODUCT_IDEA.md`, `docs/PARITY.md`.
