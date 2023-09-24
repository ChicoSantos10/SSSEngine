add_library(Directx12 MODULE
        src/Directx12.cpp
        ${CMAKE_SOURCE_DIR}/src/Win32/src/Win32Utils.cpp
)
set_target_properties(Directx12 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}/Directx12)

target_link_libraries(Directx12 PRIVATE DXGI.lib)
target_link_libraries(Directx12 PRIVATE D3D12.dll)

target_include_directories(SSSEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Directx12 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Directx12 PUBLIC ${CMAKE_SOURCE_DIR}/src/Common/include)
target_include_directories(Directx12 PUBLIC ${CMAKE_SOURCE_DIR}/include/directx)
target_include_directories(Directx12 PUBLIC ${CMAKE_SOURCE_DIR}/src/Win32/include)

add_compile_definitions($<$<CONFIG:Debug>:SSSENGINE_DEBUG_GRAPHICS>)