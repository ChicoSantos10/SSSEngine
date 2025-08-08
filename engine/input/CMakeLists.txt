add_library(SSSInput STATIC)

if(WIN32)
    target_sources(SSSInput PRIVATE win32/src/Win32Input.cpp)
    target_link_libraries(SSSInput PRIVATE Xinput9_1_0.lib)
endif()

add_subdirectory(ihi)

target_link_libraries(SSSInput PUBLIC
  SSSPlatform
)
