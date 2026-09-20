# Bitácora de decisiones del estudiante

> **Idioma:** Español (pedagógico).  
> **Ubicación:** raíz del repo `STUDENT_DECISION_LOG.md`.

**Última actualización:** 2026-09-20  
**Change relacionado:** bootstrap SDD legacy (`project-sdd-init`); sin feature OpenSpec aún

---

## 1. Resumen de la decisión

Se actualizó un plugin VST3 ya existente (BFXRVST / Bfxr Ableton) para que entre en el pipeline Spec-Driven Development: estructura `ai-specs/` + `openspec/`, estándares honestos según el código real (JUCE + FigBug/bfxr + WebView2), y docs de arquitectura. El producto sigue siendo un instrumento de SFX estilo Bfxr para Ableton en Windows, no un downgrade a sfxr clásico (sfxr.me).

---

## 2. Mapa de flujo de datos

```text
Ableton Live (MIDI, automatización, XML del set)
  → PluginProcessor (APVTS + processBlock)
      → SfxrSynth (third_party/bfxr)
          → salida de audio del host

UI nativa / WebView bfxr2
  → APVTS o BfxrWebBridge (JSON)
      → helpers del processor (preview, presets, locks)
```

- **Qué viaja:** valores normalizados de parámetros (uids de `SfxrParams`), flags `lock_*`, JSON de bfxr2 en el bridge.
- **Quién valida:** rangos min/max de `SfxrParams` y del APVTS; el bridge mapea nombres y clampea; no hay Zod porque el stack es C++.

---

## 3. Justificación de Clean Architecture

Aunque no hay carpetas `presentation/` / `application/` al estilo web, se separa por **responsabilidad**:

- **Editor / paneles:** solo UI (si mezclaras DSP aquí, romperías el hilo de audio y el host).
- **Layout + bridge:** reglas de mapeo y presets (si viven en la UI, duplicás tablas y fallás el import).
- **Processor + third_party:** realtime e infraestructura de build.

Eso permite cambiar la UI web o el layout nativo sin reescribir el sinte, y al revés.

---

## 4. Control de salida (anti-alucinación)

No hay LLM. El “schema” es el vector `SfxrParams::params` + mapas del bridge:

- Frontera: JSON bfxr2 ↔ APVTS; futuro posible: JSON jsfxr `oldParams`.
- Si el JSON viene mal: no se aplica a ciegas; se registra en Debug.
- Ejemplo crítico: `frequency_slide` ↔ `slide` con factor ×2 / ×0,5 documentado en `docs/PARITY.md`.

---

## 5. Glosario técnico (en este proyecto)

| Concepto | Qué significa aquí | Dónde aparece |
|---|---|---|
| APVTS | Árbol de parámetros JUCE que el DAW automatiza y guarda | `PluginProcessor::apvts` |
| One-shot | El sonido se arma al disparo y corre hasta el fin del envelope | `armVoiceFromApvts` / `processBlock` |
| Bridge | Conversión explícita JSON web ↔ parámetros nativos | `BfxrWebBridge.*` |
| Bfxr vs sfxr | Bfxr = set extendido (más ondas/params); sfxr.me = subset clásico | `docs/PARAM_GAP_ANALYSIS.md` |

---

## 6. Qué aprendí / qué defendería en una oral

- No confundir “herramienta web interesante” (sfxr.me) con el alcance del producto (Bfxr completo).
- En plugins, la arquitectura “limpia” se juega sobre todo en **hilos** (UI vs audio) y en **un solo motor de verdad**.
- Empaquetar WebView2 + assets locales es parte del producto, no un detalle menor (`bfxr2web`, parche Footsteppr).

---

## 7. Auditoría rápida de modularidad (bootstrap)

| Severidad | Hallazgo |
|---|---|
| Major | Mapeos web/jsfxr/APVTS repartidos; riesgo de drift si no se documentan (mitigado con PARITY + gap analysis). |
| Minor | `PluginProcessor` concentra presets + audio; un extracto “PresetService” ayudaría en cambios futuros. |
| Suggestion | Tests unitarios del bridge JSON aún no existen. |
| Suggestion | Carpetas conceptuales aún planas bajo `Source/` (aceptable para el tamaño actual). |

No se refactorizó código en este bootstrap; solo docs + estructura SDD.
