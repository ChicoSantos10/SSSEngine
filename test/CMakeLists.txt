if(BUILD_TESTING)
    file(GLOB SOURCES math/*.cpp Time/*.cpp)
    # TODO: Figure out the best way to have debug capabilities. Probably debug will be a standalone library
    add_executable(tests test_main.cpp ${SOURCES})
    target_include_directories(tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(tests PUBLIC ${CMAKE_SOURCE_DIR}/src/Platform/Common/include)

    add_test(NAME Test COMMAND tests)
endif()
