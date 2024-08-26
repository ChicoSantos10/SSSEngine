file(GLOB win32src
        "src/*.cpp"
)

# INVESTIGATE: What privacy should this be? Public, Private or Interface
target_sources(SSSEngine PUBLIC
        ${win32src}
        ${CMAKE_SOURCE_DIR}/src/Common/src/Window.cpp
        ${CMAKE_SOURCE_DIR}/src/Common/src/Application.cpp
        ${CMAKE_SOURCE_DIR}/src/Common/src/Exceptions.cpp
        ${CMAKE_SOURCE_DIR}/src/Common/src/Exceptions.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/src/Renderer.cpp
)

target_include_directories(SSSEngine PUBLIC include)
target_include_directories(SSSEngine PUBLIC ${CMAKE_SOURCE_DIR}/include/XAudioRedist)