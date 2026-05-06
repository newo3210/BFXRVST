# QA antes de release (Bfxr Ableton VST3)

Matriz mínima recomendada antes de etiquetar una versión (por ejemplo `v0.2.0`).

## Entorno

- **DAW:** Ableton Live (anotar versión probada: mínimo acordado con el equipo).
- **OS:** Windows 10 u 11 (64 bits).
- **Sample rates:** 44,1 kHz y 48 kHz (cambiar en preferencias del motor de audio y repetir pruebas críticas).

## Checklist funcional

1. **Inserción y audio:** cargar el VST3 en una pista MIDI; disparar con teclado MIDI y con el botón **Play** en la pestaña nativa.
2. **Estado guardado:** guardar el proyecto Live, cerrar, reabrir; comprobar que sliders y locks coinciden.
3. **Pestaña web:** abrir **bfxr2 (web)**; confirmar que carga la copia local `bfxr2web` (sin depender de red si la carpeta está junto al binario).
4. **Bridge web → motor:** con la pestaña **Bfxr** activa en la web, pulsar **Importar web → motor VST** y verificar que el sonido y los sliders nativos cambian de forma coherente.
5. **Bridge motor → web:** pulsar **Enviar motor → web** con la pestaña **Bfxr** activa; la UI web debe reflejar los valores (sin error en consola de depuración si se observa).
6. **Footsteppr:** en la build empaquetada, no debe aparecer la pestaña ni el script Footsteppr (parche post-build).
7. **Redimensión:** redimensionar la ventana del editor; pestañas y barras deben seguir usables.
8. **Hilos:** uso normal (sin congelaciones al mover sliders o importar); si hay glitches solo en Release con WebView2, ver notas de optimización MSVC en `docs/RELEASE.md`.

## WebView2

- **Desarrollo:** puede hacer falta el paquete NuGet `Microsoft.Web.WebView2` para compilar (CMake lo documenta al fallar).
- **Usuario final:** necesita el **WebView2 Runtime** (en muchos Windows 10/11 ya instalado). Si la pestaña web queda en blanco, instalar el runtime desde Microsoft y reabrir Live.

## Depuración

- En **Debug**, si el SR del proyecto no es 44,1 kHz, el host puede mostrar en consola un aviso del plugin (ver `docs/PARITY.md`).
