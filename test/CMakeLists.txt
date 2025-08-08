if(BUILD_TESTING)
    add_library(SSSTest STATIC Test.cpp)
    target_include_directories(SSSTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    target_link_libraries(SSSTest PUBLIC SSSUtils SSSLogging)

    add_subdirectory(math)
    add_subdirectory(time)
endif()
