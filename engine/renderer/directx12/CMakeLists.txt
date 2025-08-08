message(STATUS Building Directx12)
add_library(Directx12 MODULE
        src/Directx12.cpp
        src/RenderingContext.cpp
)

target_compile_options(Directx12 PUBLIC /DEBUG:FULL)

set(OUTPUT_DIR ${BIN_DIR}/Directx12)
set_target_properties(Directx12 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

target_include_directories(Directx12 PRIVATE internal)

# TODO: Revisit this
target_link_libraries(Directx12 
    PRIVATE 
        DXGI.lib
        D3D12.lib
        SSSPlatform
        SSSRenderer
        SSSWin32Interface
        SSSRendererInternal
        comsuppw.lib
)

setup_dx12(Directx12)
# NOTE: If we want the dxcompiler to work on other platforms we will need to either add this to the renderer itself or
# have some sort of intermediate library that we add to all platforms that want it
setup_dxcompiler(Directx12)
