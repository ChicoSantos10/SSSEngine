add_executable(SSSEngine WIN32 Win32.cpp)
target_link_options(SSSEngine PRIVATE -municode)
target_link_libraries(SSSEngine PRIVATE Xinput9_1_0.lib)