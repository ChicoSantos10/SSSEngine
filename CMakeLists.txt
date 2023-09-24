cmake_minimum_required(VERSION 3.25)
include(FetchContent)
include(ExternalProject)

project(SSSEngine)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(src)

add_compile_options(-Wall)


