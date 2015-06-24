add_executable(TreeNode TreeNode.cpp)
target_link_libraries(TreeNode osvrUtilCpp)
osvr_setup_gtest(TreeNode)
