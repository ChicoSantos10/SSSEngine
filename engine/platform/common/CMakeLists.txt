target_include_directories(SSSPlatformCommon
    INTERFACE
  include
)
target_include_directories(SSSPlatformCommonInternal
  INTERFACE
  internal
)

target_sources(SSSPlatform PRIVATE 
  src/PlatformThreading.cpp
  src/ApplicationAllocator.cpp
)
