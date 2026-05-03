add_subdirectory(defines)
add_subdirectory(utils)
add_subdirectory(traits)
add_subdirectory(system)
add_subdirectory(math)
add_subdirectory(iterator)
add_subdirectory(storage)
add_subdirectory(memory)
add_subdirectory(threading)
add_subdirectory(text)
add_subdirectory(logging)
add_subdirectory(filesystem)
add_subdirectory(containers)

add_library(SSSBase INTERFACE)
target_link_libraries(SSSBase INTERFACE
  SSSDefines
  SSSUtils
  SSSTraits
  SSSSystem
  SSSMath
  SSSStorage
  SSSMemory
  SSSThreading
  SSSText
  SSSLogging
  SSSFileSystem
  SSSIterators
  SSSContainers
)
