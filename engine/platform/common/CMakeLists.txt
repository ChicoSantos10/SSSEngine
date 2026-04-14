target_include_directories(SSSPlatformCommon
  PUBLIC
  include
)
target_include_directories(SSSPlatformCommonInternal
  INTERFACE
  internal
)

target_sources(SSSPlatformCommon PRIVATE 
  src/PlatformThreading.cpp
  src/ApplicationAllocator.cpp
)
