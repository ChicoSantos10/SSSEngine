add_executable(SSSTimerTest 
  Timer.test.cpp
)

target_link_libraries(SSSTimerTest PRIVATE
  SSSPlatform
  SSSTest
)

add_test(NAME TimeTest COMMAND SSSTimerTest)
