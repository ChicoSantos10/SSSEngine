add_library(SSSSystem INTERFACE)
target_include_directories(SSSSystem INTERFACE include)

include(TestBigEndian)
test_big_endian(IS_BIG_ENDIAN)

if(IS_BIG_ENDIAN)
  target_compile_definitions(SSSSystem INTERFACE SSSENGINE_BIG_ENDIAN)
else()
  target_compile_definitions(SSSSystem INTERFACE SSSENGINE_LITTLE_ENDIAN)
endif()
