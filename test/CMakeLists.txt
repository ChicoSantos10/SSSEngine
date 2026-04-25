if(BUILD_TESTING)
    add_library(SSSTest STATIC Test.cpp)
    target_include_directories(SSSTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    target_link_libraries(SSSTest PUBLIC 
      SSSUtils 
      SSSLogging 
      SSSBase
    )
    if(WIN32)
      target_link_libraries(SSSTest PRIVATE SSSWin32)
    else()
      target_link_libraries(SSSTest PRIVATE SSSLinux)
    endif()

    add_subdirectory(math)
    add_subdirectory(utils)
    add_subdirectory(string)
    add_subdirectory(storage)
endif()
