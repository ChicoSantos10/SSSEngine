cmake_minimum_required(VERSION 3.25)
include(FetchContent)
include(ExternalProject)

project(SSSEngine)
set(CMAKE_CXX_STANDARD 26)

# TODO: Look at all options from clang-tidy and use the best ones

add_compile_definitions(
        "$<$<CONFIG:Debug>:SSSENGINE_DEBUG;SSSENGINE_ASSERTIONS>"
        "$<$<CONFIG:Internal>:SSSENGINE_INTERNAL;SSSENGINE_ASSERTIONS>"
        "$<$<CONFIG:Release>:SSSENGINE_RELEASE>"
)

# TODO: Different compile options for different build types (DEBUG, RELEASE, ...)
if (MSVC)
    add_compile_definitions(SSSENGINE_MSVC)
    add_compile_options(/W4 /WX /Od /EHs)
    # NOTE: Disable certain warnings
    add_compile_options(
            # NOTE: Disable this on debug only
            /wd4100 /wd4189
            # NOTE: This can be always disabled
            /wd28251
    )
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



