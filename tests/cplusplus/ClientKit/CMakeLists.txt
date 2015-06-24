
foreach(test SimultaneousContexts SequentialContexts OverlappedContexts)
    add_executable(Test${test}
        ${test}.cpp)
    target_link_libraries(Test${test} osvrClientKitCpp)
    osvr_setup_gtest(Test${test})
endforeach()
