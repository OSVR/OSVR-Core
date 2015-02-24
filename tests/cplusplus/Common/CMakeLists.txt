add_executable(TestCommon Endianness.cpp Serialization.cpp)
target_link_libraries(TestCommon osvrCommon)
setup_gtest(TestCommon)
