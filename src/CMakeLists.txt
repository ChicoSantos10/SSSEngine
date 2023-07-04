add_executable(SSSEngine WIN32 Win32.cpp)

# Make a bin directory
set(BUILD_DIR ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${BUILD_DIR})

set_target_properties(SSSEngine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
target_include_directories(SSSEngine PUBLIC ${INCLUDE_DIR})
target_include_directories(SSSEngine PUBLIC ${INCLUDE_DIR}/directx)

if (MINGW)
    target_link_options(SSSEngine PRIVATE -municode)
    target_link_libraries(SSSEngine PRIVATE libksuser.a)
endif ()
target_link_libraries(SSSEngine PRIVATE DXGI.lib)
target_link_libraries(SSSEngine PRIVATE D3D12.lib)

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
    add_custom_command(TARGET SSSEngine POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${XAudio2_9_DLL} ${BUILD_DIR})
else ()
    message(FATAL_ERROR "Could not find XAudio2_9.dll at ${XAudio2_9_DLL}")
endif ()

add_compile_definitions(DEBUG)