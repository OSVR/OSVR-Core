foreach(testname TreeNode ContainerWrapper UniqueContainer Projection QuatExpMap)
    add_executable(${testname}
        ${testname}.cpp)
    target_link_libraries(${testname} osvr-catch-main)
    target_link_libraries(${testname} osvrUtilCpp)
    add_test(NAME ${testname} COMMAND ${testname})
endforeach()

target_link_libraries(Projection eigen-headers)
target_link_libraries(QuatExpMap eigen-headers vendored-vrpn)
