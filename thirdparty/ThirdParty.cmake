function(build_debug_info result)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set(${result} TRUE PARENT_SCOPE)
    else()
        set(${result} FALSE PARENT_SCOPE)
    endif()
endfunction()

function(setup_dx12 target)
    if(WIN32)
      set(AGILITY_SDK_VERSION 616 CACHE INTERNAL "Agility SDK Version")
      set(AGILITY_SDK_VERSION_MINOR 1)
      set(AGILITY_SDK_PATH ${THIRDPARTY_LIB_DIR}/MicrosoftAgilitySDK-1.${AGILITY_SDK_VERSION}.${AGILITY_SDK_VERSION_MINOR})
      set(AGILITY_SDK_DOWNLOAD agility_sdk_download)
      set(AGILITY_SDK_DLL_DIR ${AGILITY_SDK_PATH}/build/native/bin/x64)

      set(COPY_PATH ${BIN_DIR}/Directx12/D3D12)

      set(COPY_COMMAND 
          ${CMAKE_COMMAND} -E make_directory ${COPY_PATH} &&
          ${CMAKE_COMMAND} -E copy_if_different ${AGILITY_SDK_DLL_DIR}/D3D12Core.dll ${COPY_PATH}
      )

      build_debug_info(IS_DEBUG_BUILD)
      if(IS_DEBUG_BUILD)
        set(COPY_COMMAND 
          ${COPY_COMMAND} &&
          ${CMAKE_COMMAND} -E copy_if_different ${AGILITY_SDK_DLL_DIR}/D3D12Core.pdb ${COPY_PATH} &&
          ${CMAKE_COMMAND} -E copy_if_different ${AGILITY_SDK_DLL_DIR}/d3d12SDKLayers.dll ${COPY_PATH} &&
          ${CMAKE_COMMAND} -E copy_if_different ${AGILITY_SDK_DLL_DIR}/d3d12SDKLayers.pdb ${COPY_PATH}
        )
      endif()

      FetchContent_Declare(
          ${AGILITY_SDK_DOWNLOAD}
          URL "https://www.nuget.org/api/v2/package/Microsoft.Direct3D.D3D12/1.${AGILITY_SDK_VERSION}.${AGILITY_SDK_VERSION_MINOR}"
          DOWNLOAD_DIR "${AGILITY_SDK_PATH}"
          SOURCE_DIR "${AGILITY_SDK_PATH}"
          DOWNLOAD_NO_EXTRACT FALSE
          DOWNLOAD_NO_PROGRESS FALSE
      )
      FetchContent_MakeAvailable(${AGILITY_SDK_DOWNLOAD})
      
      add_custom_command(
          TARGET ${target} 
          POST_BUILD
          COMMAND ${COPY_COMMAND}
      )

      set(AGILITY_INCLUDE_DIR ${AGILITY_SDK_PATH}/build/native/include)
      target_include_directories(${target} PRIVATE 
          ${AGILITY_INCLUDE_DIR}
          ${AGILITY_INCLUDE_DIR}/d3dx12
      )
    else()
      message(FATAL_ERROR "Can only build agility sdk in windows")
    endif()
endfunction()

function(setup_dxcompiler target)
    set(DXCOMPILER_VERSION 1.8.2505.1)
    set(DXCOMPILER_URL https://github.com/microsoft/DirectXShaderCompiler/releases/download/v${DXCOMPILER_VERSION})
    if(WIN32)
        set(DXCOMPILER_URL "${DXCOMPILER_URL}/dxc_2025_07_14.zip")
        set(HASH SHA256=9ad895a6b039e3a8f8c22a1009f866800b840a74b50db9218d13319e215ea8a4)
    else(LINUX)
        set(DXCOMPILER_URL "${DXCOMPILER_URL}/linux_dxc_2025_07_14.x86_64.tar.gz")
        set(HASH SHA256=f2213da1fc99dc8778c8823078e16ba97c7f80f86a1d4520ab1adf4b462bc48c)
    endif()
    set(DXCOMPILER_DIR ${THIRDPARTY_LIB_DIR}/DxCompiler-${DXCOMPILER_VERSION})

    set(DXCOMPILER_DLL_DIR ${DXCOMPILER_DIR}/bin/x64)
    set(COPY_COMMAND 
        # ${CMAKE_COMMAND} -E make_directory TODO: Specific dir for dxcompiler
        ${CMAKE_COMMAND} -E copy_if_different ${DXCOMPILER_DLL_DIR}/dxcompiler.dll ${BIN_DIR} &&
        ${CMAKE_COMMAND} -E copy_if_different ${DXCOMPILER_DLL_DIR}/dxil.dll ${BIN_DIR}
    )

    FetchContent_Declare(
        DXCOMPILER
        URL ${DXCOMPILER_URL}
        DOWNLOAD_DIR ${DXCOMPILER_DIR}
        SOURCE_DIR ${DXCOMPILER_DIR}
        URL_HASH ${HASH}
        DOWNLOAD_NO_EXTRACT FALSE
        DOWNLOAD_NO_PROGRESS FALSE
    )
    FetchContent_MakeAvailable(DXCOMPILER)

    add_custom_command(
        TARGET ${target} 
        POST_BUILD
        COMMAND ${COPY_COMMAND}
    )

    set(DXCOMPILER_LIB_DIR ${DXCOMPILER_DIR}/lib/x64)
    target_link_libraries(${target} PRIVATE 
        ${DXCOMPILER_LIB_DIR}/dxcompiler.lib
        ${DXCOMPILER_LIB_DIR}/dxil.lib
    )

    set(DXCOMPILER_INCLUDE_DIR ${DXCOMPILER_DIR}/inc)
    target_include_directories(${target} PRIVATE 
        ${DXCOMPILER_INCLUDE_DIR}
        ${DXCOMPILER_INCLUDE_DIR}/Support
        ${DXCOMPILER_INCLUDE_DIR}/hlsl
    )
endfunction()
