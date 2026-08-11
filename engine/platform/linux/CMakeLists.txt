target_sources(SSSPlatform PRIVATE 
  src/LinuxWindowHandle.cpp
  src/LinuxDebug.cpp
  src/LinuxFile.cpp
  src/LinuxLibrary.cpp
  src/LinuxMemory.cpp
  src/LinuxTimer.cpp
  src/LinuxThreading.cpp
)
add_library(SSSLinuxInternal INTERFACE)

target_include_directories(SSSLinuxInternal INTERFACE internal)

target_link_libraries(SSSPlatform 
  PUBLIC 
  SSSPlatformCommon
  PRIVATE
  SSSLinuxInternal
  SSSPlatformCommonInternal
) 

setup_wayland(SSSPlatform)

function(CREATE_GUI_APP name)
  add_executable(${name} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/LinuxEntry.cpp)
  target_link_libraries(${name}
      PRIVATE
      SSSPlatform
      SSSLinuxInternal
      )
endfunction()
