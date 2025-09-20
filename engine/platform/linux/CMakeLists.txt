add_library(SSSLinux STATIC 
  src/LinuxWindowHandle.cpp
  src/LinuxDebug.cpp
  src/LinuxFile.cpp
  src/LinuxLibrary.cpp
  src/LinuxMemory.cpp
  src/LinuxTimer.cpp
)

add_library(SSSLinuxInterface INTERFACE)
target_include_directories(SSSLinuxInterface INTERFACE internal)

target_link_libraries(SSSLinux 
  PRIVATE 
    SSSPlatform
  PUBLIC
    SSSLinuxInterface
) 

target_link_libraries(SSSPlatform 
    INTERFACE SSSLinux
)

setup_wayland(SSSLinux)

function(CREATE_GUI_APP name)
  add_executable(${name} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/src/LinuxEntry.cpp)
    target_link_libraries(${name}
      PRIVATE
        SSSLinuxInterface
      )
endfunction()
