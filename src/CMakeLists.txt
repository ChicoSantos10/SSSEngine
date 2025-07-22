if (WIN32)
    add_compile_definitions(
            SSSENGINE_WIN32
            $<$<CONFIG:Debug,RelWithDebugInfo>:DBGHELP_TRANSLATE_TCHAR>
    )
    add_executable(SSSEngine WIN32)

    # INVESTIGATE: Are we still using DIA SDK?
    # DIA SDK - used only for debug builds
    if(NOT DEFINED ENV{VSINSTALLDIR})
        message(FATAL_ERROR "VSINSTALLDIR not set! Run vcvarsall.bat or similar")
    endif()
    set(SDK_PATH "$ENV{VSINSTALLDIR}DIA SDK")
    message(STATUS "SDK PATH IS ${SDK_PATH}")
    target_include_directories(SSSEngine PUBLIC "$<$<CONFIG:Debug,RelWithDebInfo>:${SDK_PATH}/include>")
    target_link_libraries(SSSEngine PUBLIC "$<$<CONFIG:Debug,RelWithDebInfo>:${SDK_PATH}/lib/amd64/diaguids.lib>")
    target_link_libraries(SSSEngine PRIVATE "$<$<CONFIG:Debug,RelWithDebInfo>:dbghelp.lib>")
elseif (LINUX)
    add_compile_definitions(SSSENGINE_LINUX)
endif ()

# target_precompile_headers(SSSEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ssspch.h)

# Make a bin directory
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "")
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
message(STATUS ${BIN_DIR})

set_target_properties(SSSEngine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
target_include_directories(SSSEngine PUBLIC ${INCLUDE_DIR})
target_include_directories(SSSEngine PUBLIC ${INCLUDE_DIR}/directx)

#add_library(DIRECTX_GUIDs_LIB STATIC dxguids.cpp)
#target_include_directories(DIRECTX_GUIDs_LIB PUBLIC ${INCLUDE_DIR})
#target_include_directories(DIRECTX_GUIDs_LIB PUBLIC ${INCLUDE_DIR}/directx)
#target_link_libraries(SSSEngine PRIVATE DIRECTX_GUIDs_LIB)

if (MSVC)
elseif (MINGW)
    target_link_options(SSSEngine PRIVATE -municode)
    target_link_libraries(SSSEngine PRIVATE libksuser.a)
endif ()
# INVESTIGATE: Is this the only way to ensure unicode? Is there a better way for each compiler
add_compile_definitions(UNICODE _UNICODE)
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

set(D3D12_CORE_PATH ${CMAKE_SOURCE_DIR}/libs/Microsoft.Direct3D.D3D12.1.610.5/build/native/bin/x64)
if (EXISTS ${D3D12_CORE_PATH})
    # move the dll to the build directory
    message(STATUS "Found D3D12Core path: ${D3D12_CORE_PATH}")
    file(GLOB DLL_FILES ${D3D12_CORE_PATH}/*.dll ${D3D12_CORE_PATH}/*.pdb)
    file(COPY ${DLL_FILES} DESTINATION ${BIN_DIR}/Directx12/D3D12)
else ()
    message(FATAL_ERROR "Could not find D3D12Core path at ${D3D12_CORE_PATH}")
endif ()

add_subdirectory(Platform)
add_subdirectory(Renderer)
add_subdirectory(Math)
add_subdirectory(Common)
add_subdirectory(Logging)
add_subdirectory(Input)
add_subdirectory(Audio)
