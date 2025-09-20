add_library(SSSPlatform INTERFACE)
add_subdirectory(common)

if (WIN32)
    add_subdirectory(win32)
elseif (LINUX)
    add_subdirectory(linux)
endif ()

target_link_libraries(SSSPlatform 
  INTERFACE
    SSSMath
    SSSUtils
    SSSLogging
)
