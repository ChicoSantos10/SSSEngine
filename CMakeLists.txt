cmake_minimum_required(VERSION 3.25)
include(FetchContent)
include(ExternalProject)

project(SSSEngine)
set(CMAKE_CXX_STANDARD 23)

# TODO: Add address sanitizer

add_compile_definitions(
        "$<$<CONFIG:Debug>:SSSENGINE_DEBUG;SSSENGINE_ASSERTIONS>"
        "$<$<CONFIG:Internal>:SSSENGINE_INTERNAL;SSSENGINE_ASSERTIONS>"
        $<$<CONFIG:Release>:SSSENGINE_RELEASE>
)

if (WIN32)
    add_compile_definitions(SSSENGINE_WIN32)
elseif (LINUX)
    add_compile_definitions(SSSENGINE_LINUX)
    message(FATAL_ERROR "LINUX not currently supported!")
endif ()

# TODO: Different compile options for different build types (DEBUG, RELEASE, ...)
if (MSVC)
    add_compile_options(/W4)
    # TODO: Warnings as errors, Suppress useless warnings
elseif (MINGW)
    message(FATAL_ERROR "Finish before porting to mingw")
endif ()

add_subdirectory(src)



