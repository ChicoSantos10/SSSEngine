cmake_minimum_required(VERSION 3.25)
include(FetchContent)
include(ExternalProject)

project(SSSEngine)
set(CMAKE_CXX_STANDARD 23)

# TODO: Add address sanitizer

if (WIN32)
    add_compile_definitions(SSSENGINE_WIN32)
elseif (LINUX)
    add_compile_definitions(SSSENGINE_LINUX)
    message(FATAL_ERROR "LINUX not currently supported!")
endif ()

if (MSVC)
    add_compile_options(/W4)
elseif (MINGW)
endif ()

add_subdirectory(src)



