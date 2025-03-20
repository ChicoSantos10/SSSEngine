add_library(Directx12 MODULE
        src/Directx12.cpp
        # TODO: Build library with these so we don't have to include on both main and directx etc. Make it a module?
        ${CMAKE_SOURCE_DIR}/src/Platform/Win32/src/Win32Utils.cpp
        ${CMAKE_SOURCE_DIR}/src/Platform/Win32/src/Win32Debug.cpp
)
# target_precompile_headers(Directx12 REUSE_FROM SSSEngine)

target_compile_options(Directx12 PUBLIC /DEBUG:FULL)

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
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/src/Platform/Win32/include)
target_include_directories(Directx12 BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/src/Renderer/Common/include)

target_link_libraries(Directx12 PRIVATE DXGI.lib)
target_link_libraries(Directx12 PRIVATE D3D12.lib)
#target_link_libraries(Directx12 PRIVATE D3DCompiler.lib)

add_library(DX_COMPILER SHARED IMPORTED)
set_property(TARGET DX_COMPILER PROPERTY
        IMPORTED_LOCATION "${DX12_COMPILER_PATH}/bin/x64/dxcompiler.dll"
)
set_property(TARGET DX_COMPILER PROPERTY
        IMPORTED_LOCATION "${DX12_COMPILER_PATH}/bin/x64/dxil.dll"
)
set_property(TARGET DX_COMPILER PROPERTY
        IMPORTED_IMPLIB "${DX12_COMPILER_PATH}/lib/x64/dxcompiler.lib"
)
set_target_properties(DX_COMPILER PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Directx12 PRIVATE DX_COMPILER)

add_compile_definitions($<$<CONFIG:Debug>:SSSENGINE_DEBUG_GRAPHICS>)
