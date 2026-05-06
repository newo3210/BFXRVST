# Paridad bfxr2 (web) ↔ plugin (FigBug/bfxr + JUCE)

Referencia web: [increpare/bfxr2](https://github.com/increpare/bfxr2) (MIT). Motor nativo: [FigBug/bfxr](https://github.com/FigBug/bfxr) (BSD-3-Clause).

## Alcance explícito del plugin actual

- Sin generador de pasos (no está en FigBug/bfxr); la copia embebida de bfxr2 **no registra Footsteppr** (parche post-build en `bfxr2web`).
- Windows, formato **VST3** únicamente.
- Parámetros y presets de mutación / random / plantillas clásicas alineados con **SfxrParams** del repo FigBug (equivalente al núcleo Bfxr histórico).

## Comprobado / cubierto

- Todos los parámetros expuestos en `SfxrParams::params` están en el `AudioProcessorValueTreeState`.
- Acciones: Randomize, Mutate, Pickup/Coin, Laser/Shoot, Explosion, Powerup, Hit/Hurt, Jump, Blip/Select (mismos métodos que `SfxrParams`).
- **Randomize / Mutate** respetan `lockedParams` del motor cuando los toggles **L** (nativo) están activos; `masterVolume` permanece bloqueado en el motor.
- Reproducción: nota MIDI o botón **Play** (one-shot). Opción **Re-preview al soltar slider** dispara un nuevo preview al soltar un slider (hilo de mensajes).
- Estado del plugin guardado vía XML del APVTS (incluye locks y `rePreview`).
- Pestaña **bfxr2 (web)**: WebView2 (JUCE) carga `index.html` desde la copia `bfxr2web` junto al `.dll` del plugin (post-build CMake desde el submódulo `third_party/bfxr2` + parche); si no existe, cae a `https://www.bfxr.net/`.

## Criterios de “OK” (Etapa A — motor y SR)

- **44,1 kHz:** comportamiento de referencia para comparar con bfxr2 / FigBug.
- **48 kHz:** el host entrega ese SR a `SfxrSynth::setSampleRate`; el usuario debe poder trabajar sin sorpresas graves de tono o duración. En **Debug**, si el SR ≠ 44,1 kHz se emite un `DBG` recordando que el núcleo histórico se calibró pensando en 44,1 kHz.
- **Importar web → motor:** tras importar desde la pestaña **Bfxr**, el preset debe sonar “en la misma familia” que en la web a mismo SR (diferencias menores por redondeo o por detalle de DSP son aceptables; divergencias claras de tono o forma → bug de mapeo).
- **Mapeo bridge (JS → APVTS):** en `BfxrWebBridge.cpp`: `frequency_slide` ↔ `slide` (factor ×2 al entrar, ×0,5 al salir); `waveType` con tabla JS→0–8; alias de nombres (`frequency_start`, `pitch_jump_*`, etc.) documentados en código.

## Bridge bidireccional (Etapa B)

- **Importar web → motor:** botón en `BfxrWebPanel`; JSON desde `__bfxrVstGetActiveSynthParams` → `applyBfxr2JsonToApvts`.
- **Enviar motor → web:** botón **Enviar motor → web**; JSON desde `apvtsToBfxr2JsonString` inyectado en `__bfxrVstApplyParamsObject` (llama `apply_params` + `update_ui_params` en la pestaña **Bfxr** activa).
- **Errores:** si no hay bridge, pestaña no Bfxr o excepción JS, el resultado de evaluación queda registrado en **Debug** (`DBG`) donde aplica.
- **Sincronización automática:** no forma parte del MVP; solo acciones explícitas por botón.

## Brechas conocidas / límites

| Área | Notas |
|------|--------|
| **UI vs bfxr2.net** | La web tiene layout, plantillas `.bcol`, etc. Aquí hay sliders verticales + barra de herramientas; paridad visual 1:1 no es objetivo. |
| **Cambio de parámetros durante el sonido** | Los valores se congelan al inicio del disparo (nota / Play); la opción **Re-preview** vuelve a disparar al soltar un slider. |
| **bfxr2-only** | Cualquier sinte o flujo que exista solo en el JS de bfxr2 y no en FigBug requiere porte o doble motor. |
| **MSVC Release + WebView2** | Si la vista web queda en blanco solo en Release, valorar `/O1` en Windows (ver `docs/RELEASE.md`). |

## Cómo seguir el checklist

1. Ejecutar bfxr2 en local y enumerar pantallas y acciones (solo **Bfxr** en build empaquetada).
2. Por cada acción, anotar si llama solo a `SfxrParams` / `SfxrSynth` o código exclusivo de JS.
3. Si es exclusivo de JS, decidir: portar a C++, descartar, o embebér WebView con bridge.
4. Completar la matriz en `docs/QA.md` antes de un tag de release.
