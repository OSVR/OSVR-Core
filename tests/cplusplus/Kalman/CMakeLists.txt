
foreach(test KalmanConstruction KalmanNoNaNs)
    add_executable(Test${test}
        ${test}.cpp
        $<TARGET_OBJECTS:osvr-catch-main>)
    target_link_libraries(Test${test} osvrKalman eigen-headers osvr-catch2-interface osvr_cxx11_flags)
    add_test(NAME Test${test} COMMAND Test${test})
endforeach()

add_executable(Kalman_ManualTest ContentsInvalid.h ManualTest.cpp)
target_link_libraries(Kalman_ManualTest osvrKalman eigen-headers osvr_cxx11_flags)
