add_executable(SSSTextTest
  String.test.cpp
)

target_link_libraries(SSSTextTest PRIVATE
  SSSTest
  SSSText
)

add_test(NAME TextTest COMMAND SSSTextTest)
