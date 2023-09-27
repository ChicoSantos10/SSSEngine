add_library(Directx12 MODULE
        src/Directx12.cpp
        ${CMAKE_SOURCE_DIR}/src/Win32/src/Win32Utils.cpp
)
set_target_properties(Directx12 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}/Directx12)
message(STATUS ${BIN_DIR}/Directx12)

#target_include_directories(BEFORE SSSEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/src/Common/include)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/include/directx/dxguids)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/libs/Microsoft.Direct3D.D3D12.1.610.5/build/native/include)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/libs/Microsoft.Direct3D.D3D12.1.610.5/build/native/include/d3dx12)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/libs)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/src/Win32/include)

target_link_libraries(Directx12 PRIVATE DXGI.lib)
target_link_libraries(Directx12 PRIVATE D3D12.lib)

add_compile_definitions($<$<CONFIG:Debug>:SSSENGINE_DEBUG_GRAPHICS>)