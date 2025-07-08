if(BUILD_TESTING)
    file(GLOB SOURCES math/*.cpp)
    # TODO: Figure out the best way to have debug capabilities. Probably debug will be a standalone library
    add_executable(tests test_main.cpp ${SOURCES})

    add_test(NAME Test COMMAND tests)
endif()
