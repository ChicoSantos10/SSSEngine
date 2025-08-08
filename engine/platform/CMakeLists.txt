add_library(SSSPlatform INTERFACE)
add_subdirectory(Common)

if (WIN32)
    add_subdirectory(Win32)
elseif (LINUX)
    message(FATAL_ERROR "LINUX not currently supported!")
endif ()

target_link_libraries(SSSPlatform 
  INTERFACE
    SSSMath
    SSSUtils
    SSSLogging
)
