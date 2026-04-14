add_executable(SSSUtilsTest
  Bits.test.cpp
  Storage.test.cpp
)

target_link_libraries(SSSUtilsTest PRIVATE
  SSSUtils
  SSSTest
)

add_test(NAME UtilsTest COMMAND SSSUtilsTest)
