cmake_minimum_required(VERSION 3.22)

if(NOT WIN32)
    message(FATAL_ERROR "BfxrAbleton: este arbol de CMake esta pensado solo para Windows (VST3 + WebView2).")
endif()

project(BfxrAbleton VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JUCE_BUILD_VST3 ON CACHE BOOL "" FORCE)
set(JUCE_BUILD_VST2 OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_AU OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_AUV3 OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_RTAS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_UNITY OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
)
FetchContent_MakeAvailable(JUCE)

add_library(bfxr_core STATIC
    third_party/bfxr/SfxrParams.cpp
    third_party/bfxr/Util.cpp
)
target_include_directories(bfxr_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/bfxr")
if(MSVC)
    target_compile_options(bfxr_core PRIVATE /W3)
endif()

juce_add_plugin(BfxrAbleton
    COMPANY_NAME "BfxrOpen"
    BUNDLE_ID com.bfxropen.bfxrableton
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE BfxO
    PLUGIN_CODE BfxA
    FORMATS VST3
    PRODUCT_NAME "Bfxr Ableton"
    NEEDS_WEBVIEW2 TRUE)

juce_generate_juce_header(BfxrAbleton)

target_sources(BfxrAbleton
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/BfxrParameterLayout.cpp
        Source/NativeParamsPanel.cpp
        Source/BfxrWebPanel.cpp)

target_include_directories(BfxrAbleton PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/bfxr")

target_compile_definitions(BfxrAbleton
    PUBLIC
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(BfxrAbleton
    PRIVATE
        bfxr_core
        juce::juce_audio_utils
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

add_custom_command(
    TARGET BfxrAbleton
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf "$<TARGET_FILE_DIR:BfxrAbleton>/bfxr2web"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/third_party/bfxr2"
            "$<TARGET_FILE_DIR:BfxrAbleton>/bfxr2web"
    COMMENT "Copiando bfxr2 (submodulo) junto al binario del plugin para WebView2"
    VERBATIM)
