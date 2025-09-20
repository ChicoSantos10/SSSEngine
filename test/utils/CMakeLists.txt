add_executable(SSSUtilsTest
  Bits.test.cpp
)

target_link_libraries(SSSUtilsTest PRIVATE
  SSSUtils
  SSSTest
)

add_test(NAME UtilsTest COMMAND SSSUtilsTest)
