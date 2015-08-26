foreach(testname TreeNode TypePack ContainerWrapper UniqueContainer Projection)
    add_executable(${testname} ${testname}.cpp)
    target_link_libraries(${testname} osvrUtilCpp)
    setup_gtest(${testname})
endforeach()

target_link_libraries(Projection eigen-headers)
