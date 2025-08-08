add_executable(${EDITOR_NAME} WIN32 Win32Editor.cpp)
target_link_libraries(${EDITOR_NAME} 
    PRIVATE 
        SSSWin32Interface
        SSSEditor
    )
  # TODO: This will need to be in every exe that builds dx12 
  target_compile_definitions(${EDITOR_NAME} PRIVATE SSSENGINE_AGILITY_SDK_VERSION=${AGILITY_SDK_VERSION})
