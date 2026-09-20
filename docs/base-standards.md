---

## description: Development rules for BFXRVST (Bfxr Ableton VST3). Applies to all AI agents.
alwaysApply: true

## 1. Core Principles

- **Small tasks, one at a time**: Prefer baby steps; one focused change per OpenSpec apply slice.
- **Test-Driven Development**: Prefer failing tests / checklists before behaviour changes. For audio DSP, use automated tests when feasible; otherwise document scenarios in `docs/QA.md` / acceptance matrix before claiming done.
- **Type Safety**: Prefer strong typing (C++ types, JUCE parameter IDs, explicit JSON field maps). Avoid silent magic numbers without named constants.
- **Clear Naming**: Descriptive names for parameters, bridge fields, and UI actions.
- **Incremental Changes**: Prefer incremental patches over large rewrites of `third_party/`.
- **Question Assumptions**: Especially sample-rate behaviour, WebView2 Release quirks, and web↔motor mapping.
- **Pattern Detection**: Highlight duplicated mapping tables (APVTS ↔ SfxrParams ↔ bfxr2 JSON ↔ jsfxr).

## 2. Language Standards

- **English Only** for technical artifacts:
  - Code (identifiers, comments that document behaviour, log/`DBG` messages)
  - Documentation (`README.md`, `docs/*`, OpenSpec artifacts, `ARCHITECTURE_SDD.md`)
  - Git commit messages (conventional commits preferred)
  - CI workflow names and scripts that are project-owned

**Exception — pedagogical UX:**

- `STUDENT_DECISION_LOG.md` (repo root) is written in **Spanish**.
- User-facing chat may be Spanish when the user writes in Spanish.
- Some existing UI strings are Spanish (`Re-preview…`); new host-visible strings may stay Spanish if consistent with the editor UX, but **docs and OpenSpec stay English**.

## 3. Specific standards

- [Plugin / audio engine standards](./backend-standards.md) — JUCE processor, `SfxrSynth`, APVTS, threading, third_party policy
- [Data Architecture Discovery](./data-architecture-discovery.md) — access-pattern gate (plugin state / export; no server DB today)
- [UI / editor standards](./frontend-standards.md) — native JUCE panels, WebView2, bridge UX
- [OpenSpec Tasks Mandatory Steps](./openspec-tasks-mandatory-steps.md) — required checklist when writing `tasks.md`

Product / parity references (not “coding standards”, but required reading for feature work):

- `docs/PRODUCT_IDEA.md`, `docs/PARITY.md`, `docs/PARAM_GAP_ANALYSIS.md`, `docs/SFXR_ME_PARAMS.md`, `docs/QA.md`, `docs/RELEASE.md`

## 4. Project Skills

- Skills live in `ai-specs/skills` (canonical).
- `.cursor/skills` mirrors via symlink (or copy-fallback on Windows).
- When a request matches a skill, load and follow that `SKILL.md` before continuing.

## 5. Planning Model Requirement

Planning workflows should use the highest-reasoning model available in the session when the skill asks for it (`enrich-us`, OpenSpec plan/ff/continue). If the session model differs, prefer correcting to the skill’s requirement rather than inventing shortcuts.

## 6. Symlink Integrity and Multi-Agent Portability

- Canonical reusable artifacts live under `ai-specs/`.
- Keep `.cursor/skills` and `.cursor/agents` pointing at `ai-specs` (symlink preferred; physical copy if symlink fails).
- After adding/removing skills, run `sync-agent-symlinks` (or equivalent).

## 7. Mandatory OpenSpec Artifact Updates for Post-Apply Changes

After `/opsx:apply` and before `/opsx:archive`, treat new requests as **spec updates first**:

1. Update OpenSpec change artifacts (`specs/`, `design.md`, `tasks.md`).
2. Continue / re-plan if needed.
3. Implement only after artifacts match the request.
4. Re-verify before archive.

## 8. Project Director Entry Point

- Default orchestration: **`/director`** + skill `project-director`.
- Assisted OpenSpec: no global `openspec` CLI required; agents read/write `openspec/changes/` and `openspec/specs/`.
- Human checkpoints: plan approval, verify review, explicit OK before archive.
- Adversarial review must not be self-performed by the same apply session.
- Archive gate: updated root `ARCHITECTURE_SDD.md` + `STUDENT_DECISION_LOG.md`.

## 9. Clean Architecture — Conceptual Layer Mapping (this repo)

Map each OpenSpec change to conceptual layers; declare **concrete paths in that change’s `design.md`**.

| Conceptual layer | Responsibility in BFXRVST | Concrete paths (today) |
|---|---|---|
| **Presentation** | Editor UI only: layout, tabs, buttons, WebView host. No DSP in paint/resized. | `Source/PluginEditor.*`, `Source/NativeParamsPanel.*`, `Source/BfxrWebPanel.*` |
| **Application / services** | Parameter sync, presets/mutate/randomize orchestration, web↔APVTS bridge mapping, trigger/preview policy. | `Source/BfxrParameterLayout.*`, `Source/BfxrWebBridge.*`, preset helpers on `PluginProcessor` |
| **Infrastructure / audio** | Realtime audio callback, MIDI → one-shot, synth mutex, host state XML, build packaging of `bfxr2web`. | `Source/PluginProcessor.*` (`processBlock`), `third_party/bfxr/*`, `cmake/`, `.github/workflows/` |
| **Contracts** | Parameter IDs / ranges (`SfxrParams::params`), APVTS layout, bfxr2 JSON field map, (future) jsfxr `oldParams` JSON. | `third_party/bfxr/SfxrParams.h`, bridge maps in `BfxrWebBridge.cpp`, docs `PARAM_GAP_ANALYSIS.md` |

### 9.1 Output control & errors

- Validate / clamp parameter values through `SfxrParams` min/max and APVTS ranges.
- Bridge JSON: prefer explicit field maps; log failures with `DBG` in Debug; do not crash the host on bad JS.
- No LLM boundaries in this product today; if AI is added later, put schemas at the service boundary.

### 9.2 UI note

`NativeParamsPanel` / `BfxrWebPanel` collect input and call processor/bridge helpers. They must not own synthesis math or mutate `SfxrSynth` directly from the UI thread without going through the processor’s locking API.

### 9.3 Third-party policy

- Prefer **not** forking behaviour of `third_party/bfxr` or `third_party/bfxr2` unless required for Ableton/VST packaging.
- Document patches (e.g. `cmake/patch-bfxr2-vst.ps1` removing Footsteppr) in `docs/PARITY.md` / `docs/RELEASE.md`.

## 10. Mandatory Root Documentation Artefacts

Update when a feature/module is built or finalized:

### 10.1 `ARCHITECTURE_SDD.md` (English)

Template: `ai-specs/skills/project-director/references/architecture-sdd-template.md`.

### 10.2 `STUDENT_DECISION_LOG.md` (Spanish)

Template: `ai-specs/skills/project-director/references/student-decision-log-template.md`.

### 10.3 Pipeline enforcement

| Phase | Requirement |
|---|---|
| Plan | `design.md` states layer mapping + contracts; `tasks.md` includes root-doc updates |
| Apply | Implementer updates both root docs when the slice lands |
| Verify / adversarial | Missing/stale root docs → CRITICAL / Major |
| Archive | Hard gate: both root docs present and updated when required |
