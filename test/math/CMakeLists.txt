add_executable(SSSMathTest 
    Matrix.test.cpp
    Vector.test.cpp
)

target_link_libraries(SSSMathTest PRIVATE
  SSSMath
  SSSPlatform
  SSSTest
)

add_test(NAME MathTest COMMAND SSSMathTest)
