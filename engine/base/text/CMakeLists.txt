add_library(SSSText INTERFACE)
target_include_directories(SSSText INTERFACE include)

target_link_libraries(SSSText INTERFACE
  SSSUtils
  SSSSystem
  SSSMath
  SSSMemory
)
