# Bfxr Ableton (BFXRVST)

Plugin **VST3** para Windows que integra el motor [**FigBug/bfxr**](https://github.com/FigBug/bfxr) en **JUCE 8**, con panel nativo y pestaña **bfxr2** embebida por **WebView2**.

## Qué incluye

- **Pestaña Nativo (VST):** sliders verticales para todos los parámetros de `SfxrParams`, presets tipo Bfxr (coin, laser, explosion, etc.), **Randomize** / **Mutate**, reproducción por MIDI o **Play**.
- **Locks (L):** cada parámetro (excepto volumen maestro) puede bloquearse para que Randomize/Mutate no lo cambien; el motor fuerza `masterVolume` bloqueado internamente.
- **Re-preview al soltar slider:** opción para disparar de nuevo el sonido al soltar un slider (sin re-armar en tiempo real durante el arrastre).
- **Pestaña bfxr2 (web):** copia local `bfxr2web` junto al binario (submódulo [increpare/bfxr2](https://github.com/increpare/bfxr2) + parche post-build).
- **Bridge web → motor:** botón **Importar web → motor VST** (solo pestaña **Bfxr** activa en la UI web).
- **Bridge motor → web:** botón **Enviar motor → web** (`apply_params` + actualización de UI en JS).
- **Sin Footsteppr:** el generador de pasos no se registra en la copia embebida (parche en `cmake/patch-bfxr2-vst.ps1`).
- **Estado del proyecto:** guardado/restaurado vía XML del APVTS (locks incluidos).

## Requisitos

| Rol | Requisito |
|-----|-----------|
| **Compilar** | Windows 64 bits, Visual Studio 2022 (MSVC), CMake ≥ 3.22 |
| **WebView2 SDK** | Paquete NuGet `Microsoft.Web.WebView2` (para enlazar; ver `docs/RELEASE.md`) |
| **Usar el plugin** | Windows 10+, [WebView2 Runtime](https://developer.microsoft.com/microsoft-edge/webview2/) en la máquina (típico en Win10/11 actualizado) |

## Compilar

```powershell
# Descargar WebView2 SDK para CMake (si no usas Visual Studio + NuGet GUI)
New-Item -ItemType Directory -Force packages | Out-Null
Invoke-WebRequest -Uri "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe" -OutFile packages\nuget.exe
.\packages\nuget.exe install Microsoft.Web.WebView2 -OutputDirectory packages -Version 1.0.3485.44

git submodule update --init --recursive

cmake -B build -G "Visual Studio 17 2022" -A x64 "-DJUCE_WEBVIEW2_PACKAGE_LOCATION=$PWD\packages"
cmake --build build --config Release
```

Tras compilar, CMake copia `third_party/bfxr2` a `bfxr2web` junto al `.dll` del plugin y aplica el parche VST. Detalle de versiones y CI: **`docs/RELEASE.md`**.

## Probar en Ableton Live

1. **Instalar el VST3**  
   El build con `COPY_PLUGIN_AFTER_BUILD TRUE` puede copiar a  
   `C:\Program Files\Common Files\VST3\Bfxr Ableton.vst3`.  
   Si compilas sin admin, copia manualmente la carpeta  
   `Bfxr Ableton.vst3` desde  
   `build\BfxrAbleton_artefacts\Release\VST3\`  
   a una carpeta que Live escanee (por ejemplo la carpeta de plugins VST3 del usuario).

2. **En Ableton**  
   - *Preferencias* → *Plug-Ins* → activa **Usar carpetas personalizadas de plug-ins VST3** si usas una ruta propia y añade esa carpeta.  
   - Pulsa **Reescanear** (o reinicia Live tras copiar el plugin).

3. **Crear una pista MIDI**  
   - En el navegador, *Plug-ins* → *VST3* → **Bfxr Ableton** (o el nombre que muestre Live).  
   - Abre la ventana del instrumento.

4. **Pestaña Nativo**  
   - Dispara con el **teclado MIDI** o el botón **Play**.  
   - Prueba presets y **Randomize** / **Mutate**; activa **L** en algunos parámetros y comprueba que no se mueven.  
   - Opcional: activa **Re-preview al soltar slider** y mueve un slider.

5. **Pestaña bfxr2 (web)**  
   - Debe cargar la UI local si existe `bfxr2web` junto al `.dll`. Si ves la web remota o pantalla en blanco, revisa WebView2 Runtime y que la carpeta `bfxr2web` esté en el mismo directorio que el binario del plugin.  
   - Con la pestaña **Bfxr** activa en la web: **Importar web → motor VST** y **Enviar motor → web**.

6. **Sample rates**  
   Prueba el proyecto del Live a **44,1 kHz** y **48 kHz** (preferencias de audio) si quieres validar paridad; ver **`docs/PARITY.md`** y **`docs/QA.md`**.

## Documentación del repo

| Archivo | Contenido |
|---------|-----------|
| `docs/PARITY.md` | Paridad web ↔ motor, criterios de OK, bridges |
| `docs/QA.md` | Checklist previa a release |
| `docs/RELEASE.md` | CI, NuGet, instalación, notas MSVC/WebView2 |

## CI

GitHub Actions (`.github/workflows/windows-vst3.yml`): build Release en Windows, artefacto **`BfxrAbleton-VST3-Windows.zip`** con el `.vst3`.

## Licencias / terceros

- Motor: **FigBug/bfxr** (BSD-3-Clause), en `third_party/bfxr`.  
- UI web: **bfxr2** (MIT), submódulo `third_party/bfxr2`.  
- **JUCE** vía FetchContent (lee `CMakeLists.txt`).

## Versión

Definida en CMake: `project(BfxrAbleton VERSION …)` en `CMakeLists.txt`.
