add_executable(PathTree PathTree.cpp)
target_link_libraries(PathTree osvrRouting)
setup_gtest(PathTree)
