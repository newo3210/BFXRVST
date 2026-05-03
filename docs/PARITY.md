# Paridad bfxr2 (web) ↔ plugin (FigBug/bfxr + JUCE)

Referencia web: [increpare/bfxr2](https://github.com/increpare/bfxr2) (MIT). Motor nativo: [FigBug/bfxr](https://github.com/FigBug/bfxr) (BSD-3-Clause).

## Alcance explícito del plugin actual

- Sin generador de pasos (no está en FigBug/bfxr).
- Windows, formato **VST3** únicamente.
- Parámetros y presets de mutación / random / plantillas clásicas alineados con **SfxrParams** del repo FigBug (equivalente al núcleo Bfxr histórico).

## Comprobado / cubierto

- Todos los parámetros expuestos en `SfxrParams::params` están en el `AudioProcessorValueTreeState`.
- Acciones: Randomize, Mutate, Pickup/Coin, Laser/Shoot, Explosion, Powerup, Hit/Hurt, Jump, Blip/Select (mismos métodos que `SfxrParams`).
- Reproducción: nota MIDI o botón **Play** (one-shot).
- Estado del plugin guardado vía XML del APVTS.
- Pestaña **bfxr2 (web)**: WebView2 (JUCE) carga `index.html` desde la copia `bfxr2web` junto al `.dll` del plugin (post-build CMake desde el submódulo `third_party/bfxr2`); si no existe, cae a `https://www.bfxr.net/`.

## Brechas conocidas (trabajo “completo” pendiente)

| Área | Notas |
|------|--------|
| **UI vs bfxr2.net** | La web tiene layout, bloqueo de parámetros, plantillas `.bcol`, etc. Aquí hay sliders verticales + barra de herramientas; hay que portar o embebér la UI si se exige paridad visual 1:1. |
| **Bloqueo de parámetros** | `lockedParams` en `SfxrParams` no está expuesto en la UI del plugin. |
| **Sample rate** | FigBug documenta que parte del código asume **44,1 kHz**; el plugin fija el sinte al SR del host. Auditar tono y envolventes a 48 kHz. |
| **Cambio de parámetros durante el sonido** | Los valores se congelan al inicio del disparo (nota / Play); no hay re-síntesis en vivo por slider. |
| **bfxr2-only** | Cualquier sinte, preset o flujo que exista solo en el JS de bfxr2 y no en FigBug requiere porte o doble motor. |
| **Bridge web ↔ APVTS** | Botón **Importar web -> motor VST** lee `synth.params` de la pestaña **Bfxr** activa en bfxr2, mapea nombres JS→FigBug y escribe el APVTS. **Play motor VST** dispara el sinte nativo. Sigue sin sincronización automática ni envío VST→web. |
| **Ocultar pasos en web** | El submódulo incluye el generador de pasos de bfxr2; ocultarlo implica parche JS/CSS en el árbol de `bfxr2` o build fork. |

## Cómo seguir el checklist

1. Ejecutar bfxr2 en local y enumerar pantallas y acciones (sin pasos).
2. Por cada acción, anotar si llama solo a `SfxrParams` / `SfxrSynth` o código exclusivo de JS.
3. Si es exclusivo de JS, decidir: portar a C++, descartar, o embebér WebView con bridge.
