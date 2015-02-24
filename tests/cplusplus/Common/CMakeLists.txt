add_executable(TestCommon Endianness.cpp Serialization.cpp)
target_link_libraries(TestCommon osvrCommon tinympl)
setup_gtest(TestCommon)
