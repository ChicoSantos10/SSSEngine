add_library(Directx12 MODULE
        src/Directx12.cpp
        ${CMAKE_SOURCE_DIR}/src/Win32/src/Win32Utils.cpp
)
set(OUTPUT_DIR ${BIN_DIR}/Directx12)
set_target_properties(Directx12 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

set(DX12_LIB_PATH ${CMAKE_SOURCE_DIR}/libs/Microsoft.Direct3D.D3D12.1.610.5)
set(DX12_COMPILER_PATH ${CMAKE_SOURCE_DIR}/libs/dxCompiler)

file(GLOB DX_COMPILER_DLL
        ${DX12_COMPILER_PATH}/bin/x64/*.dll
        ${DX12_COMPILER_PATH}/lib/x64/*.lib
)

file(COPY ${DX_COMPILER_DLL}
        DESTINATION ${BIN_DIR}
)

#target_include_directories(BEFORE SSSEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/src/Common/include)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/include/directx/dxguids)
target_include_directories(Directx12 BEFORE PUBLIC ${DX12_LIB_PATH}/build/native/include)
target_include_directories(Directx12 BEFORE PUBLIC ${DX12_LIB_PATH}/build/native/include/d3dx12)
target_include_directories(Directx12 BEFORE PUBLIC ${DX12_COMPILER_PATH}/inc)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/libs)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/src/Win32/include)

target_link_libraries(Directx12 PRIVATE DXGI.lib)
target_link_libraries(Directx12 PRIVATE D3D12.lib)
#target_link_libraries(Directx12 PRIVATE D3DCompiler.lib)

add_library(DX_COMPILER SHARED IMPORTED)
set_property(TARGET DX_COMPILER PROPERTY
        IMPORTED_LOCATION "N:/C++Projects/SSSEngine/libs/dxCompiler/bin/x64/dxcompiler.dll"
)
set_property(TARGET DX_COMPILER PROPERTY
        IMPORTED_LOCATION "N:/C++Projects/SSSEngine/libs/dxCompiler/bin/x64/dxil.dll"
)
set_property(TARGET DX_COMPILER PROPERTY
        IMPORTED_IMPLIB "N:/C++Projects/SSSEngine/libs/dxCompiler/lib/x64/dxcompiler.lib"
)
set_target_properties(DX_COMPILER PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Directx12 PRIVATE DX_COMPILER)

add_compile_definitions($<$<CONFIG:Debug>:SSSENGINE_DEBUG_GRAPHICS>)