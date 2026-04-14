add_library(SSSEditor STATIC src/Application.cpp)

target_link_libraries(SSSEditor PUBLIC 
    SSSCore
    SSSRenderer
    SSSAudio
    SSSInput
)

CREATE_GUI_APP(${EDITOR_NAME})

target_link_libraries(${EDITOR_NAME} 
    PRIVATE 
        SSSEditor
)

target_include_directories(SSSEditor PUBLIC include)

target_compile_definitions(SSSEditor PUBLIC SSSENGINE_EDITOR)
