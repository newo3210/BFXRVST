# jsfxr / sfxr.me parameter catalog

Reference: [sfxr.me](https://sfxr.me) ([chr15m/jsfxr](https://github.com/chr15m/jsfxr)), a JavaScript port of classic **sfxr** (DrPetter).

This document catalogs the serialized `Params` JSON (`oldParams: true`) used by jsfxr / sfxr.me. Values are the **normalized UI/synth unit** unless noted. Most continuous fields live on **\[0, 1\]**; fields marked **SIGNED** live on **\[−1, 1\]**.

Source of truth in upstream code: `sfxr.js` → `function Params()`.

Last updated: 2026-09-20

---

## Format overview

| Aspect | Detail |
|--------|--------|
| Structure flag | `oldParams: true` marks classic sfxr layout (as opposed to any newer layout) |
| Wave types | Integer **0–3** only (square / saw / sine / noise) |
| Continuous params | ~23 `p_*` fields + `wave_type` |
| Export extras | `sound_vol`, `sample_rate`, `sample_size` (not part of Base58 payload) |
| Interchange | Full object via `fromJSON` / JSON; compact share string via Base58 (`toB58` / `fromB58`) |

Example shape (defaults):

```json
{
  "oldParams": true,
  "wave_type": 0,
  "p_env_attack": 0,
  "p_env_sustain": 0.3,
  "p_env_punch": 0,
  "p_env_decay": 0.4,
  "p_base_freq": 0.3,
  "p_freq_limit": 0,
  "p_freq_ramp": 0,
  "p_freq_dramp": 0,
  "p_vib_strength": 0,
  "p_vib_speed": 0,
  "p_arp_mod": 0,
  "p_arp_speed": 0,
  "p_duty": 0,
  "p_duty_ramp": 0,
  "p_repeat_speed": 0,
  "p_pha_offset": 0,
  "p_pha_ramp": 0,
  "p_lpf_freq": 1,
  "p_lpf_ramp": 0,
  "p_lpf_resonance": 0,
  "p_hpf_freq": 0,
  "p_hpf_ramp": 0,
  "sound_vol": 0.5,
  "sample_rate": 44100,
  "sample_size": 8
}
```

---

## Wave type

| Field | Type | Range | Default | Meaning |
|-------|------|-------|---------|---------|
| `wave_type` | int | **0–3** | `0` | Oscillator shape |

| Value | Name | Constant in jsfxr |
|------:|------|-------------------|
| 0 | Square | `SQUARE` |
| 1 | Sawtooth | `SAWTOOTH` |
| 2 | Sine | `SINE` |
| 3 | Noise | `NOISE` |

Classic sfxr has **no** triangle / pink / tan / whistle / breaker waves (those exist only in Bfxr; see `PARAM_GAP_ANALYSIS.md`).

---

## Envelope

| Field | Range | Default | Meaning |
|-------|-------|---------|---------|
| `p_env_attack` | \[0, 1\] | `0` | Attack time (volume rises to peak) |
| `p_env_sustain` | \[0, 1\] | `0.3` | Sustain time (hold at peak / punch region) |
| `p_env_punch` | \[0, 1\] | `0` | Sustain punch (extra amplitude tilt at sustain start) |
| `p_env_decay` | \[0, 1\] | `0.4` | Decay / release time (volume falls to silence) |

Internal timing (for reference): attack/sustain/decay lengths are derived roughly as `v * v * 100000` samples at the synth’s working rate.

---

## Tone / frequency

| Field | Range | Default | Meaning |
|-------|-------|---------|---------|
| `p_base_freq` | \[0, 1\] | `0.3` | Start frequency (base pitch) |
| `p_freq_limit` | \[0, 1\] | `0` | Minimum frequency cutoff (slide stops here; prevents endless dive) |
| `p_freq_ramp` | **SIGNED** \[−1, 1\] | `0` | Frequency slide (pitch glide) |
| `p_freq_dramp` | **SIGNED** \[−1, 1\] | `0` | Delta slide (acceleration of the frequency slide) |

---

## Vibrato

| Field | Range | Default | Meaning |
|-------|-------|---------|---------|
| `p_vib_strength` | \[0, 1\] | `0` | Vibrato depth |
| `p_vib_speed` | \[0, 1\] | `0` | Vibrato rate |

**Note:** `mutate()` in jsfxr still touches a legacy `p_vib_delay` in some builds, but that field is **not** part of the `Params` constructor, **not** in Base58 `params_order`, and should be ignored for interchange.

---

## Pitch jump / arpeggio

| Field | Range | Default | Meaning |
|-------|-------|---------|---------|
| `p_arp_mod` | **SIGNED** \[−1, 1\] | `0` | Pitch jump amount (up or down) |
| `p_arp_speed` | \[0, 1\] | `0` | How quickly the pitch jump happens |

Classic sfxr has a **single** pitch jump. Bfxr adds a second jump pair + repeat (see gap analysis).

---

## Square duty

| Field | Range | Default | Meaning |
|-------|-------|---------|---------|
| `p_duty` | \[0, 1\] | `0` | Square-wave duty (high vs low time); mainly meaningful for `wave_type === 0` |
| `p_duty_ramp` | **SIGNED** \[−1, 1\] | `0` | Duty sweep over time |

---

## Repeat

| Field | Range | Default | Meaning |
|-------|-------|---------|---------|
| `p_repeat_speed` | \[0, 1\] | `0` | Note repeat rate (certain synth state resets each repeat) |

---

## Flanger / phaser offset

| Field | Range | Default | Meaning |
|-------|-------|---------|---------|
| `p_pha_offset` | **SIGNED** \[−1, 1\] | `0` | Flanger offset (second delayed copy of the wave) |
| `p_pha_ramp` | **SIGNED** \[−1, 1\] | `0` | Flanger sweep |

---

## Filters

| Field | Range | Default | Meaning |
|-------|-------|---------|---------|
| `p_lpf_freq` | \[0, 1\] | `1` | Low-pass cutoff (`1` ≈ open / no filtering) |
| `p_lpf_ramp` | **SIGNED** \[−1, 1\] | `0` | Low-pass cutoff sweep |
| `p_lpf_resonance` | \[0, 1\] | `0` | Low-pass resonance |
| `p_hpf_freq` | \[0, 1\] | `0` | High-pass cutoff |
| `p_hpf_ramp` | **SIGNED** \[−1, 1\] | `0` | High-pass cutoff sweep |

---

## Volume and sample export

| Field | Range / values | Default | Meaning |
|-------|----------------|---------|---------|
| `sound_vol` | typically \[0, 1\] (UI); applied with `exp(sound_vol) - 1` gain | `0.5` | Overall sound volume |
| `sample_rate` | Hz (commonly `44100`, also `22050`, `11025`, `5512` in UI) | `44100` | WAV / generator sample rate |
| `sample_size` | bits per sample (`8` or `16`) | `8` | WAV bit depth |

These three are useful for game-dev export and JSON round-trips. They are **not** encoded in the Base58 share string (see below).

---

## Base58 interchange

jsfxr can compress the classic synth definition into a URL-friendly **Base58** string (`Params.prototype.toB58` / `fromB58`, helpers `sfxr.b58encode` / `sfxr.b58decode`).

| Detail | Value |
|--------|--------|
| Alphabet | `123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz` (Bitcoin-style; no `0OIl`) |
| Payload order (`params_order`) | `wave_type`, then the 22 `p_*` fields listed above (envelope → tone → vibrato → arp → duty → repeat → flanger → LPF → HPF) |
| Encoding | `wave_type` as a single byte; each `p_*` as IEEE-754 float bits little-endian (4 bytes), then the byte array is Base58-encoded |
| **Not** in Base58 | `oldParams`, `sound_vol`, `sample_rate`, `sample_size` |

Signed params for documentation / UI clamping:

`p_freq_ramp`, `p_freq_dramp`, `p_arp_mod`, `p_duty_ramp`, `p_pha_offset`, `p_pha_ramp`, `p_lpf_ramp`, `p_hpf_ramp`.

---

## Presets (game templates)

jsfxr / sfxr.me expose random generators for typical game SFX. Each resets/randomizes the classic fields (not Bfxr-only params).

| Method | Role |
|--------|------|
| `pickupCoin` | Coin / pickup blip |
| `laserShoot` | Laser / shoot |
| `explosion` | Explosion |
| `powerUp` | Power-up |
| `hitHurt` | Hit / hurt |
| `jump` | Jump |
| `blipSelect` | UI blip / select |
| `synth` | Longer synth-ish tone (jsfxr extension) |
| `tone` | Steady 440 Hz-ish sine tone (jsfxr extension) |
| `click` | Click / tick (jsfxr extension) |
| `random` | Fully randomized classic params |
| `mutate` | Small random deltas on existing params |

BFXRVST’s native UI maps the classic set (Pickup/Coin, Laser/Shoot, Explosion, Powerup, Hit/Hurt, Jump, Blip/Select, Randomize, Mutate) via FigBug `SfxrParams` methods — same *family*, Bfxr-extended engine underneath.

---

## Field count checklist

| Category | Count |
|----------|------:|
| `wave_type` | 1 |
| Envelope `p_env_*` | 4 |
| Tone `p_base_freq` / limit / ramp / dramp | 4 |
| Vibrato | 2 |
| Arp / pitch jump | 2 |
| Duty | 2 |
| Repeat | 1 |
| Flanger | 2 |
| LPF | 3 |
| HPF | 2 |
| **Classic synth fields (wave + p_\*)** | **23** |
| Plus `sound_vol` / `sample_rate` / `sample_size` | +3 |
| Plus `oldParams` flag | +1 |

For mapping into BFXRVST / FigBug `SfxrParams` uids, see **`docs/PARAM_GAP_ANALYSIS.md`**.
