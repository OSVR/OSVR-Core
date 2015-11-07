foreach(testname TreeNode ContainerWrapper UniqueContainer Projection)
    add_executable(${testname} ${testname}.cpp)
    target_link_libraries(${testname} osvrUtilCpp)
    osvr_setup_gtest(${testname})
endforeach()

target_link_libraries(Projection eigen-headers)
