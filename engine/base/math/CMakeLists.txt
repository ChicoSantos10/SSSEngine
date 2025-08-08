add_library(SSSMath INTERFACE)
target_include_directories(SSSMath INTERFACE include)

target_link_libraries(SSSMath INTERFACE
  SSSUtils
)
