add_executable(SSSEngine WIN32 Win32.cpp
        Win32/src/Win32Utils.cpp
        Common/src/Window.cpp
        Win32/src/Win32Window.cpp
        Common/src/Application.cpp
        Renderer/Directx12/src/CommandQueue.cpp
        Renderer/Directx12/src/Device.cpp
        Renderer/Directx12/src/Renderer.cpp
        Common/src/Exceptions.cpp
        Common/include/Exceptions.h
)

add_subdirectory(Renderer/Directx12) # TODO: Add renderer first then there add the directx12
add_subdirectory(Win32)
add_subdirectory(Common)

# Make a bin directory
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin PARENT_SCOPE)
file(MAKE_DIRECTORY ${BIN_DIR})

set_target_properties(SSSEngine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
target_include_directories(SSSEngine PUBLIC ${INCLUDE_DIR})
target_include_directories(SSSEngine PUBLIC ${INCLUDE_DIR}/directx)

add_library(DIRECTX_GUIDs_LIB STATIC dxguids.cpp)
target_include_directories(DIRECTX_GUIDs_LIB PUBLIC ${INCLUDE_DIR})
target_include_directories(DIRECTX_GUIDs_LIB PUBLIC ${INCLUDE_DIR}/directx)
target_link_libraries(SSSEngine PRIVATE DIRECTX_GUIDs_LIB)

if (MINGW)
    target_link_options(SSSEngine PRIVATE -municode)
    target_link_libraries(SSSEngine PRIVATE libksuser.a)
elseif (MSVC)
    add_compile_definitions(UNICODE _UNICODE)
endif ()
target_link_libraries(SSSEngine PRIVATE DXGI.lib) #TODO: This do not need to link to SSSEngine
target_link_libraries(SSSEngine PRIVATE D3D12.lib)
target_link_libraries(SSSEngine PRIVATE Comctl32.lib) # Windows subclasses

find_library(XAUDIO_REDIST XAudio2_9redist.lib PATHS ${CMAKE_SOURCE_DIR}/libs/XAudioRedist)
if (XAUDIO_REDIST)
    message(STATUS "Found XAudio2.lib: ${XAUDIO_REDIST}")
    target_link_libraries(SSSEngine PUBLIC ${XAUDIO_REDIST})
else ()
    message(FATAL_ERROR "Could not find XAudio2.lib")
endif ()

find_library(XAPOBASE_REDIST xapobaseredist.lib PATHS ${CMAKE_SOURCE_DIR}/libs/XAudioRedist)
if (XAPOBASE_REDIST)
    message(STATUS "Found XAPOBase.lib: ${XAPOBASE_REDIST}")
    target_link_libraries(SSSEngine PUBLIC ${XAPOBASE_REDIST})
else ()
    message(FATAL_ERROR "Could not find XAPOBase.lib")
endif ()

find_library(XAPOBASE_MD_REDIST xapobaseredist_md.lib PATHS ${CMAKE_SOURCE_DIR}/libs/XAudioRedist)
if (XAPOBASE_MD_REDIST)
    message(STATUS "Found XAPOBase_MD.lib: ${XAPOBASE_MD_REDIST}")
    target_link_libraries(SSSEngine PUBLIC ${XAPOBASE_MD_REDIST})
else ()
    message(FATAL_ERROR "Could not find XAPOBase_MD.lib")
endif ()

target_link_libraries(SSSEngine PUBLIC Xinput9_1_0.lib)

# get the redist dll at the libs folder
set(XAudio2_9_DLL ${CMAKE_SOURCE_DIR}/libs/XAudioRedist/xaudio2_9redist.dll)
if (EXISTS ${XAudio2_9_DLL})
    message(STATUS "Found XAudio2.dll: ${XAudio2_9_DLL}")
    # move the dll to the build directory
    add_custom_command(TARGET SSSEngine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${XAudio2_9_DLL} ${BIN_DIR})
else ()
    message(FATAL_ERROR "Could not find XAudio2_9.dll at ${XAudio2_9_DLL}")
endif ()

set(D3D12_CORE ${CMAKE_SOURCE_DIR}/libs/Directx12/D3D12Core.dll)
if (EXISTS ${D3D12_CORE})
    message(STATUS "Found D3D12Core.dll: ${D3D12_CORE}")
    # move the dll to the build directory
    set(COPY_PATH ${BIN_DIR}/Directx12/D3D12)
    file(COPY ${D3D12_CORE} DESTINATION ${COPY_PATH})
else ()
    message(FATAL_ERROR "Could not find D3D12Core.dll at ${D3D12_CORE}")
endif ()

set(D3D12_CORE_SDK ${CMAKE_SOURCE_DIR}/libs/Directx12/d3d12SDKLayers.dll)
if (EXISTS ${D3D12_CORE_SDK})
    message(STATUS "Found d3d12SDKLayers.dll: ${D3D12_CORE_SDK}")
    # move the dll to the build directory
    set(COPY_PATH ${BIN_DIR}/Directx12/D3D12)
    file(COPY ${D3D12_CORE_SDK} DESTINATION ${COPY_PATH})
else ()
    message(FATAL_ERROR "Could not find d3d12SDKLayers.dll at ${D3D12_CORE_SDK}")
endif ()


add_compile_definitions($<$<CONFIG:Debug>:SSSENGINE_DEBUG> $<$<CONFIG:Release>:SSSENGINE_RELEASE>)
target_precompile_headers(SSSEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ssspch.h)