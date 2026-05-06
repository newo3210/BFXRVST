# Release (Bfxr Ableton VST3, Windows)

## Versión

- La versión del proyecto CMake está en `CMakeLists.txt` (`project(BfxrAbleton VERSION …)`).
- Al publicar, crear un tag Git acorde (por ejemplo `v0.2.0`) y generar el ZIP desde CI o desde una build local Release.

## Requisitos de compilación

- Windows 64 bits, **Visual Studio 2022** (toolchain MSVC), **CMake** ≥ 3.22.
- Submódulos Git inicializados (`third_party/bfxr2`, JUCE vía FetchContent).
- **WebView2 SDK para el linker:** instalar el paquete NuGet y pasar la ruta a CMake, por ejemplo:

```powershell
# Si no tienes `nuget` en PATH, descarga nuget.exe (o usa Visual Studio > NuGet).
New-Item -ItemType Directory -Force packages | Out-Null
Invoke-WebRequest -Uri "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe" -OutFile packages\nuget.exe
.\packages\nuget.exe install Microsoft.Web.WebView2 -OutputDirectory packages -Version 1.0.3485.44
cmake -B build -G "Visual Studio 17 2022" -A x64 "-DJUCE_WEBVIEW2_PACKAGE_LOCATION=$PWD\packages"
cmake --build build --config Release
```

Tras compilar, CMake copia `third_party/bfxr2` a `<carpeta del .dll>/bfxr2web` y ejecuta `cmake/patch-bfxr2-vst.ps1` para el producto embebido (sin Footsteppr).

## CI (GitHub Actions)

- El workflow `.github/workflows/windows-vst3.yml` construye en `windows-latest`, instala WebView2 por NuGet, genera Release y sube `BfxrAbleton-VST3-Windows.zip` como artefacto.

## Instalación para usuarios

1. Copiar `Bfxr Ableton.vst3` a la carpeta de plugins VST3 del sistema o del usuario (por ejemplo `%COMMONPROGRAMFILES%\VST3` o la ruta que use Ableton).
2. Asegurar **WebView2 Runtime** (Microsoft) si la pestaña web no carga.
3. En Live: **Preferencias → Plug-ins → Rescan** (o equivalente) tras copiar el plugin.

## Nota sobre optimización MSVC + WebView2

En algunas versiones de MSVC con `/Ox` o `/O2` se han reportado pantallas en blanco con WebView2 en Release. Si ocurre, probar `/O1` solo en Windows para el target del plugin (ver incidencias públicas de JUCE sobre WebView2 y optimización).
