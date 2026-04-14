add_library(
    SSSThreading
    INTERFACE
)

target_include_directories(SSSThreading INTERFACE include)
target_link_libraries(SSSThreading 
    INTERFACE 
    SSSUtils
    SSSMemory
)
