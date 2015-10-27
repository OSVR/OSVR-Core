
osvr_convert_json(org_osvr_filter_videoimufusion_json
    org_osvr_filter_videoimufusion.json
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_filter_videoimufusion_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(KALMAN_SOURCES
    AbsoluteOrientationMeasurement.h
    AbsolutePoseMeasurement.h
    ExternalQuaternion.h
    FlexibleKalmanBase.h
    FlexibleKalmanFilter.h
    PoseDampedConstantVelocity.h
    PoseState.h)

osvr_add_plugin(NAME org_osvr_filter_videoimufusion
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    org_osvr_filter_videoimufusion.cpp
    RunningData.h
    VideoIMUFusion.h
    VideoIMUFusion.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_filter_videoimufusion_json.h"
    ${KALMAN_SOURCES})

target_link_libraries(org_osvr_filter_videoimufusion
    osvr::osvrAnalysisPluginKit
    eigen-headers
    jsoncpp_lib)

target_compile_options(org_osvr_filter_videoimufusion
    PRIVATE
    ${OSVR_CXX11_FLAGS})

set_target_properties(org_osvr_filter_videoimufusion PROPERTIES
    FOLDER "OSVR Plugins")

if(BUILD_TESTING)
    foreach(test
        AbsoluteOrientationMeasurement
        AbsolutePoseMeasurement)

        add_executable(KalmanBuildTest_${test}
            ${test}Test.cpp
            ${KALMAN_SOURCES})
        target_link_libraries(KalmanBuildTest_${test}
            eigen-headers)
        target_compile_options(KalmanBuildTest_${test}
            PRIVATE
            ${OSVR_CXX11_FLAGS})

        set_target_properties(KalmanBuildTest_${test} PROPERTIES
            FOLDER "OSVR Plugins/Kalman Filter Build Tests")
    endforeach()
endif()
