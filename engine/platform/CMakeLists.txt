add_library(SSSPlatform STATIC)
add_library(SSSPlatformCommon INTERFACE)
add_library(SSSPlatformCommonInternal INTERFACE)

add_subdirectory(common)

if (WIN32)
    add_subdirectory(win32)
elseif (LINUX)
    add_subdirectory(linux)
endif ()


target_link_libraries(SSSPlatformCommon
    INTERFACE
    SSSBase
    SSSMath
    SSSUtils
    SSSLogging
    SSSText
    SSSMemory
    SSSThreading
    SSSFileSystem
    SSSSystem
)
