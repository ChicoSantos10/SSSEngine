add_executable(SSSStorageTest
  Storage.test.cpp
  AlignedStorage.test.cpp
  Variant.test.cpp
)

target_link_libraries(SSSStorageTest PRIVATE
  SSSStorage
  SSSTest
)

add_test(NAME StorageTest COMMAND SSSStorageTest)
