add_library(SSSRenderer STATIC 
    rhi/src/Renderer.cpp 
)
target_compile_definitions(SSSRenderer PUBLIC $<$<CONFIG:Debug>:SSSENGINE_DEBUG_GRAPHICS>)

add_subdirectory(rhi)

if (WIN32)
    add_subdirectory(directx12)
endif ()

target_link_libraries(SSSRenderer 
    PUBLIC
        SSSPlatform
    PRIVATE
        SSSRendererInternal 
)
