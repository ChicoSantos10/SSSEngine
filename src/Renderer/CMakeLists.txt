target_include_directories(SSSEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (WIN32)
    add_subdirectory(Directx12)
endif ()
add_subdirectory(Common)