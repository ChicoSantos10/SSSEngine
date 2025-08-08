add_library(SSSCore STATIC)

add_subdirectory(gameobjects)
add_subdirectory(window)

target_link_libraries(SSSCore PUBLIC 
    SSSAudio 
    SSSInput
    SSSRenderer
    SSSPlatform
)
