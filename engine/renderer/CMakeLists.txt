add_library(SSSRenderer STATIC 
    rhi/src/Renderer.cpp 
)
target_compile_definitions(SSSRenderer PUBLIC $<$<CONFIG:Debug>:SSSENGINE_DEBUG_GRAPHICS>)

add_subdirectory(rhi)

if (BUILD_DX12)
    add_subdirectory(directx12)
endif ()
if (BUILD_VULKAN)
    add_subdirectory(vulkan)
endif ()

target_link_libraries(SSSRenderer 
    PUBLIC
        SSSPlatform
    PRIVATE
        SSSRendererInternal 
)
