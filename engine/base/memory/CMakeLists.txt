add_library(SSSMemory INTERFACE)
target_include_directories(SSSMemory INTERFACE include)
target_link_libraries(SSSMemory INTERFACE 
  SSSUtils
  SSSMath
)
add_subdirectory(allocator)
