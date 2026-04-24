add_library(SSSStorage INTERFACE)
target_include_directories(SSSStorage 
  INTERFACE 
    include
)
target_link_libraries(SSSStorage 
    INTERFACE 
    SSSMath
)
