cmake_minimum_required(VERSION 3.25)
include(FetchContent)
include(ExternalProject)

project(SSSEngine)
set(CMAKE_CXX_STANDARD 23)

# TODO: Add address sanitizer

if (MSVC)
    add_compile_options(/Wall)
elseif (MINGW)
    add_compile_options(-Wall)
endif ()

add_subdirectory(src)



