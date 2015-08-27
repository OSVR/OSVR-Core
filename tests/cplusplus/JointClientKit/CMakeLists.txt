
foreach(test JointClientKit)
    add_executable(Test${test}
        ${test}.cpp)
    target_link_libraries(Test${test} osvrClientKitCpp osvrJointClientKit osvr_cxx11_flags)
    setup_gtest(Test${test})
endforeach()
