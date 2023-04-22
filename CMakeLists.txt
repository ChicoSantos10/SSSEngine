cmake_minimum_required(VERSION 3.25)

project(SSSEngine)

set(CMAKE_CXX_STANDARD 23)

add_executable(SSSEngine WIN32 src/Win32.cpp src/Win32.cpp)
target_link_options(SSSEngine PRIVATE -municode)

