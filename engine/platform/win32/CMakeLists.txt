add_library(SSSWin32 STATIC 
    src/Win32Debug.cpp
    src/Win32Library.cpp
    src/Win32Timer.cpp
    src/Win32Utils.cpp
    src/Win32WindowHandle.cpp
    src/Win32Memory.cpp
    src/Win32File.cpp
)

add_library(SSSWin32Interface INTERFACE)
target_include_directories(SSSWin32Interface INTERFACE internal)

target_link_libraries(SSSWin32 
  PRIVATE 
    Comctl32.lib # Windows subclasses
    SSSPlatform
    SSSWin32Interface
) 

target_link_libraries(SSSPlatform 
    INTERFACE SSSWin32
)

function(CREATE_GUI_APP name)
      add_executable(${name} WIN32 Win32Editor.cpp)
      target_link_libraries(${name} 
          PRIVATE 
              SSSWin32Interface
          )

      # NOTE: This will need to be in every exe that builds dx12 
      target_compile_definitions(${name} PRIVATE SSSENGINE_AGILITY_SDK_VERSION=${AGILITY_SDK_VERSION})
function()

