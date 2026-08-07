# add_library(SSSPlatformCommon STATIC)
add_library(SSSPlatformCommon STATIC)
add_library(SSSPlatformCommonInternal INTERFACE)
add_subdirectory(common)

if (WIN32)
    add_subdirectory(win32)
elseif (LINUX)
    add_subdirectory(linux)
endif ()

target_link_libraries(SSSPlatformCommon
    PUBLIC
    SSSBase
    SSSMath
    SSSUtils
    SSSLogging
    SSSText
    SSSMemory
    SSSThreading
    SSSFileSystem
    SSSSystem
    PRIVATE
    SSSPlatformCommonInternal
)
