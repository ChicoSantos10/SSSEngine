add_library(SSSLogging INTERFACE)
target_include_directories(SSSLogging INTERFACE include)

target_link_libraries(SSSLogging INTERFACE
  SSSUtils
)

target_compile_definitions(SSSLogging INTERFACE $<$<BOOL:${ALLOW_LOGGING}>:SSSENGINE_LOGGING>)
