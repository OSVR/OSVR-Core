foreach(testname TreeNode ContainerWrapper UniqueContainer Projection)
    add_executable(${testname}
        ${testname}.cpp)
    target_link_libraries(${testname} osvr-catch-main)
    target_link_libraries(${testname} osvrUtilCpp)
    add_test(NAME ${testname} COMMAND ${testname})
endforeach()

target_link_libraries(Projection eigen-headers)

if(NOT MSVC OR (MSVC_VERSION LESS 1810))
    # Newer MSVC breaks this old gtest.
    # This one still on gtest for now
    add_executable(QuatExpMap QuatExpMap.cpp)
    target_link_libraries(QuatExpMap osvrUtilCpp eigen-headers vendored-vrpn)
    osvr_setup_gtest(QuatExpMap)
endif()
