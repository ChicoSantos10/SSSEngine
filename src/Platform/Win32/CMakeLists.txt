# INVESTIGATE: What privacy should this be? Public, Private or Interface
target_sources(SSSEngine PUBLIC
        src/Win32.cpp
        src/Win32Utils.cpp
        src/Win32Window.cpp
        src/Win32Debug.cpp
        ${CMAKE_SOURCE_DIR}/src/Common/src/Window.cpp
        ${CMAKE_SOURCE_DIR}/src/Common/src/Application.cpp
        ${CMAKE_SOURCE_DIR}/src/Common/src/Exceptions.cpp
        ${CMAKE_SOURCE_DIR}/src/Common/src/Exceptions.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/src/Renderer.cpp
)

target_include_directories(SSSEngine PUBLIC include)