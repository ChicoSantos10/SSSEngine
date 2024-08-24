cmake_minimum_required(VERSION 3.25)
include(FetchContent)
include(ExternalProject)

project(SSSEngine)
set(CMAKE_CXX_STANDARD 26)

add_compile_definitions(
        "$<$<CONFIG:Debug>:SSSENGINE_DEBUG;SSSENGINE_ASSERTIONS>"
        "$<$<CONFIG:Internal>:SSSENGINE_INTERNAL;SSSENGINE_ASSERTIONS>"
        "$<$<CONFIG:Release>:SSSENGINE_RELEASE>"
)

if (WIN32)
    add_compile_definitions(SSSENGINE_WIN32)
elseif (LINUX)
    add_compile_definitions(SSSENGINE_LINUX)
    message(FATAL_ERROR "LINUX not currently supported!")
endif ()

# TODO: Different compile options for different build types (DEBUG, RELEASE, ...)
# TODO: Warnings as errors, Suppress useless warnings
# TODO: Code analysis
# TODO: Add address sanitizer
if (MSVC)
    add_compile_definitions(SSSENGINE_MSVC)
    add_compile_options(/W4)
elseif (MINGW)
    add_compile_definitions(SSSENGINE_MINGW)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CLANG TRUE)
        add_compile_definitions(SSSENGINE_CLANG)
        message(FATAL_ERROR "Finish before porting to CLANG")
    else ()
        set(GCC TRUE)
        add_compile_definitions(SSSENGINE_GCC)
        message(FATAL_ERROR "Finish before porting to GCC")
    endif ()
endif ()

add_subdirectory(src)



