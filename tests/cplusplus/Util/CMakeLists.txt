foreach(testname TreeNode ContainerWrapper UniqueContainer Projection QuatExpMap Directories ExpandEnvironmentVariables)
    add_executable(${testname} ${testname}.cpp)
    target_link_libraries(${testname} osvrUtilCpp)
    osvr_setup_gtest(${testname})
endforeach()

target_link_libraries(Projection eigen-headers)
target_link_libraries(QuatExpMap eigen-headers vendored-vrpn)
target_link_libraries(Directories boost_filesystem ${Boost_SYSTEM_LIBRARIES})

