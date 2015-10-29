
foreach(test KalmanConstruction KalmanNoNaNs)
    add_executable(Test${test}
        ${test}.cpp)
    target_link_libraries(Test${test} osvrKalman eigen-headers osvr_cxx11_flags)
    osvr_setup_gtest(Test${test})
endforeach()

add_executable(Kalman_ManualTest ContentsInvalid.h ManualTest.cpp)
target_link_libraries(Kalman_ManualTest osvrKalman eigen-headers osvr_cxx11_flags)
