file(GLOB win32src
        "src/*.cpp"
)

set_target_properties(SSSEngine PROPERTIES 
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

# INVESTIGATE: What privacy should this be? Public, Private or Interface
# TODO: Only should add the sources relevant to windows
target_sources(SSSEngine PUBLIC
        ${win32src}
        ${CMAKE_SOURCE_DIR}/src/Common/src/Window.cpp
        ${CMAKE_SOURCE_DIR}/src/Common/src/Application.cpp
        ${CMAKE_SOURCE_DIR}/src/Common/src/Exceptions.cpp
        ${CMAKE_SOURCE_DIR}/src/Renderer/src/Renderer.cpp
)

target_include_directories(SSSEngine PUBLIC include)
target_include_directories(SSSEngine PUBLIC ${CMAKE_SOURCE_DIR}/include/XAudioRedist)
