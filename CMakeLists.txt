cmake_minimum_required(VERSION 3.25)
include(FetchContent)
include(ExternalProject)

project(SSSEngine)

# TODO: This works for now but we shouldn't specify it here. Instead we should tell cmake what features we need for each
# target
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BIN_DIR ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "")
file(MAKE_DIRECTORY ${BIN_DIR})
                            
# TODO: When creating dlls on folders, and having other things like the launcher exe we should specify the output
# directory for each library
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR})

set(EDITOR_NAME SSSEngine)
set(LAUNCHER_NAME SSSEngineLauncher)

option(ALLOW_FORCE_INLINE "Allow force inlining" ON)
option(ALLOW_FORCE_INLINE_STATEMENT "Allow force inlining in statements" ON)
option(ALLOW_NO_INLINE "Allow Force NO Inline" ON)
option(ALLOW_HINT_INLINE_STATEMENT "Allow inline statement hint" ON)
option(ALLOW_NO_INLINE_STATEMENT "Allow Force NO inline statement" ON)

option(ALLOW_LOGGING "Allow logging" ON)

add_compile_definitions(
        "$<$<CONFIG:Debug>:SSSENGINE_DEBUG;SSSENGINE_ASSERTIONS>"
        "$<$<CONFIG:RelWithDebInfo>:SSSENGINE_INTERNAL;SSSENGINE_ASSERTIONS>"
        "$<$<CONFIG:Release>:SSSENGINE_RELEASE>"

        "$<$<BOOL:${ALLOW_FORCE_INLINE_STATEMENT}>:SSSENGINE_ALLOW_FORCE_INLINE_STATEMENT>"
        "$<$<BOOL:${ALLOW_FORCE_INLINE}>:SSSENGINE_ALLOW_FORCE_INLINE>"
        "$<$<BOOL:${ALLOW_NO_INLINE}>:SSSENGINE_ALLOW_NO_INLINE>"
        "$<$<BOOL:${ALLOW_HINT_INLINE_STATEMENT}>:SSSENGINE_ALLOW_HINT_INLINE_STATEMENT>"
        "$<$<BOOL:${ALLOW_NO_INLINE_STATEMENT}>:SSSENGINE_ALLOW_NO_INLINE_STATEMENT>"
)

if (MSVC)
    add_compile_definitions(SSSENGINE_MSVC)

    # INVESTIGATE: What kind of exception handling we want?
    # Do we always want exceptions or maybe only in debug builds. Is it even feasible without littering code with macros
    # everywhere?
    add_compile_options(/EHsc)
    add_compile_options("$<$<CONFIG:Debug>:/W4;/WX;/Od;>")
    add_compile_options("$<$<CONFIG:RelWithDebInfo>:/W4;/WX;/O2;>")
    # NOTE: Disable certain warnings
    add_compile_options(
            "$<$<CONFIG:Debug,RelWithDebugInfo>:/wd4100;/wd4189>"
            /wd28251
    )
elseif (MINGW)
    add_compile_definitions(SSSENGINE_MINGW)
    add_compile_options(-Wall -Wextra -Wpedantic)
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

if (WIN32)
    add_compile_definitions(
            SSSENGINE_WIN32
            UNICODE
            _UNICODE
            # INVESTIGATE: Do we need this? Does this need to be defined everywhere?
            $<$<CONFIG:Debug,RelWithDebugInfo>:DBGHELP_TRANSLATE_TCHAR>
    )

elseif (LINUX)
    add_compile_definitions(SSSENGINE_LINUX)
endif ()

add_subdirectory(thirdparty)
add_subdirectory(engine)

include(CTest)
add_subdirectory(test)

add_subdirectory(scripts)
