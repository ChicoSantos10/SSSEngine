add_library(SSSAudio STATIC)

if (WIN32)
    target_sources(SSSAudio PRIVATE 
        win32/src/Win32Audio.cpp)
elseif(LINUX)
    target_sources(SSSAudio PRIVATE
        linux/src/LinuxAudio.cpp)
endif()

add_subdirectory(ahi)

target_link_libraries(SSSAudio PRIVATE 
    SSSPlatform
)

