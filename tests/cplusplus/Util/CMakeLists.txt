foreach(testname TreeNode TypePack ContainerWrapper)
    add_executable(${testname} ${testname}.cpp)
    target_link_libraries(${testname} osvrUtilCpp)
    setup_gtest(${testname})
endforeach()


add_executable(ProjectionSample ProjectionSample.cpp)
target_link_libraries(ProjectionSample osvrUtilCpp eigen-headers)