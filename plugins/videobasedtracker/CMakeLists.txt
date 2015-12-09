# This generates a header file, from the named json file, containing a string literal
# named com_osvr_VideoBasedHMDTracker_json (not null terminated)
# The file must be added as a source file to some target (as below) to be generated.
osvr_convert_json(com_osvr_VideoBasedHMDTracker_json
    com_osvr_VideoBasedHMDTracker.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoBasedHMDTracker_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/DirectShowCameraLib")
if(WIN32)
    option(OSVR_FPE
        "Enable floating-point exceptions - only works with MSVC and should never be used for shipping code!"
        OFF)
    mark_as_advanced(OSVR_FPE)
endif()

# Make sure we can link these upcoming static libraries into a shared lib
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

###
# Build a separate static library with the core of the video-based tracker
###
add_library(vbtracker-core STATIC
    BeaconBasedPoseEstimator.cpp
    BeaconBasedPoseEstimator_Kalman.cpp
    BeaconBasedPoseEstimator.h
    cvToEigen.h
    GetCameraMatrix.h
    HDKLedIdentifier.cpp
    HDKLedIdentifier.h
    HDKLedIdentifierFactory.cpp
    HDKLedIdentifierFactory.h
    IdentifierHelpers.h
    ImagePointMeasurement.h
    LedIdentifier.cpp
    LedIdentifier.h
    LED.cpp
    LED.h
    ProjectPoint.h
    Types.h
    VideoBasedTracker.cpp
    VideoBasedTracker.h)

target_link_libraries(vbtracker-core
    PUBLIC
    ${VIDEOTRACKER_EXTRA_LIBS}
    opencv_core
    osvrUtilCpp # for typedefs and boost headers
    osvrKalman
    vendored-vrpn # for quatlib
    eigen-headers)
target_compile_options(vbtracker-core
    PUBLIC
    ${OSVR_CXX11_FLAGS})

set_target_properties(vbtracker-core PROPERTIES
    FOLDER "OSVR Plugins/Video-Based Tracker")


if(WIN32)
    add_subdirectory(DirectShowCameraLib)
endif()
###
# Build the plugin
###
set(PLUGIN_SOURCES
    com_osvr_VideoBasedHMDTracker.cpp
    Oculus_DK2.cpp
    Oculus_DK2.h
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoBasedHMDTracker_json.h"
    CameraParameters.h
    CVImageSource.cpp
    DK2ImageSource.cpp
    ImageSource.cpp
    ImageSource.h
    ImageSourceFactories.h
    FakeImageSource.cpp)
if(WIN32)
    list(APPEND PLUGIN_SOURCES
        DirectShowHDKCameraFactory.h
        DirectShowImageSource.cpp
        DirectShowToCV.h)
endif()

osvr_add_plugin(NAME com_osvr_VideoBasedHMDTracker
    MANUAL_LOAD
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    ${PLUGIN_SOURCES}
)

target_link_libraries(com_osvr_VideoBasedHMDTracker
    vbtracker-core
    vendored-hidapi
    jsoncpp_lib
)
if(WIN32)
    target_link_libraries(com_osvr_VideoBasedHMDTracker directshow-camera)
    add_executable(ViewTrackingCamera
        DirectShowToCV.h
        ViewTrackingCamera.cpp)
    target_link_libraries(ViewTrackingCamera PRIVATE
        opencv_core
        opencv_highgui
        ${VIDEOTRACKER_EXTRA_LIBS}
        directshow-camera)
    set_target_properties(ViewTrackingCamera PROPERTIES
        FOLDER "OSVR Plugins/Video-Based Tracker")
endif()

set_target_properties(com_osvr_VideoBasedHMDTracker PROPERTIES
    FOLDER "OSVR Plugins/Video-Based Tracker")

if(BUILD_TESTING)
    add_executable(vbtracker-cam
        TestStandalone.cpp)
    target_link_libraries(vbtracker-cam
        PRIVATE
        vbtracker-core)
    set_target_properties(vbtracker-cam PROPERTIES
        FOLDER "OSVR Plugins/Video-Based Tracker")
    #osvr_setup_gtest(vbtracker-cam)

    if(WIN32)
        target_link_libraries(vbtracker-cam PRIVATE directshow-camera)
    endif()
endif()


if(WIN32 AND OSVR_FPE)
    set(msg_type WARNING)
    if(OSVR_OFFICIAL)
        # set in our CI
        set(msg_type FATAL_ERROR)
    endif()
    message(${msg_type} "OSVR_FPE enabled, will affect the video-based tracker plugin! Do not ship these binaries!")

    target_compile_definitions(com_osvr_VideoBasedHMDTracker PRIVATE OSVR_FPE)
    target_link_libraries(com_osvr_VideoBasedHMDTracker FloatExceptions)
    foreach(tgt vbtracker-cam vbtracker-core)
        if(TARGET ${tgt})
            target_compile_definitions(${tgt} PRIVATE OSVR_FPE)
            target_link_libraries(${tgt} PRIVATE FloatExceptions)
        endif()
    endforeach()
endif()
