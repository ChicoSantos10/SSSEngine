add_library(SSSEditor STATIC src/Application.cpp)

if(WIN32)
    add_subdirectory(win32)
else()
endif()

target_link_libraries(SSSEditor PUBLIC 
    SSSCore
)
target_include_directories(SSSEditor PUBLIC include)

# TODO: This should propagate to projects
target_compile_definitions(SSSEditor PUBLIC SSSENGINE_EDITOR)
