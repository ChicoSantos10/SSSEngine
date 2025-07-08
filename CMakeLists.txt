cmake_minimum_required(VERSION 3.25)
include(FetchContent)
include(ExternalProject)

project(SSSEngine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(
        "$<$<CONFIG:Debug>:SSSENGINE_DEBUG;SSSENGINE_ASSERTIONS>"
        "$<$<CONFIG:RelWithDebInfo>:SSSENGINE_INTERNAL;SSSENGINE_ASSERTIONS>"
        "$<$<CONFIG:Release>:SSSENGINE_RELEASE>"

        "SSSENGINE_ALLOW_FORCE_INLINE_STATEMENT"
        "SSSENGINE_ALLOW_INLINE_HINT"
        "SSSENGINE_ALLOW_FORCE_INLINE"
        "SSSENGINE_ALLOW_NO_INLINE"
        "SSSENGINE_ALLOW_HINT_INLINE_STATEMENT"
        "SSSENGINE_ALLOW_NO_INLINE_STATEMENT"
)

if (MSVC)
    add_compile_definitions(SSSENGINE_MSVC)

    add_compile_options("$<$<CONFIG:Debug>:/W4;/WX;/Od;/EHs;/EHc->")
    add_compile_options("$<$<CONFIG:RelWithDebInfo>:/W4;/WX;/O2;/EHs;/EHc->")
    # NOTE: Disable certain warnings
    add_compile_options(
            "$<$<CONFIG:Debug,RelWithDebugInfo>:/wd4100;/wd4189>"
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

# TODO: What are the common include folders? Can we use the target_include_directories but using dependencies to include
# everything necessary?
include_directories(BEFORE src/Common/include src/Math/include)

include(CTest)
add_subdirectory(test)

add_subdirectory(src)



