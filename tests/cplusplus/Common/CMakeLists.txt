add_executable(TestCommon Serialization.cpp)
target_link_libraries(TestCommon osvrCommon)
setup_gtest(TestCommon)
