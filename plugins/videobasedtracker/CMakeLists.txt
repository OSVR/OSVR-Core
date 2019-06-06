# This generates a header file, from the named json file, containing a string literal
# named com_osvr_VideoBasedHMDTracker_json (not null terminated)
# The file must be added as a source file to some target (as below) to be generated.
osvr_convert_json(com_osvr_VideoBasedHMDTracker_json
    com_osvr_VideoBasedHMDTracker.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoBasedHMDTracker_json.h")

# Be able to find our generated header file, have shared files find us, and have us find shared files.
include_directories("${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${OSVR_VIDEOTRACKERSHARED_INCLUDE_DIR}")
if(WIN32)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/DirectShowCameraLib")
endif()

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
    HDKLedIdentifier.cpp
    HDKLedIdentifier.h
    HDKLedIdentifierFactory.cpp
    HDKLedIdentifierFactory.h
    ImagePointMeasurement.h
    LedIdentifier.cpp
    LedIdentifier.h
    LED.cpp
    LED.h
    Types.h
    VideoBasedTracker.cpp
    VideoBasedTracker.h
    ${OSVR_VIDEOTRACKERSHARED_SOURCES_CORE}
    ${OSVR_VIDEOTRACKERSHARED_SOURCES_HDKDATA})

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

if(NOT WIN32)
    find_package(libuvc REQUIRED)
    find_package(Libusb1 REQUIRED)
endif()

###
# Build the plugin
###
set(PLUGIN_SOURCES
    com_osvr_VideoBasedHMDTracker.cpp
    Oculus_DK2.cpp
    Oculus_DK2.h
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoBasedHMDTracker_json.h"
    CVImageSource.cpp
    DK2ImageSource.cpp
    ConfigurationParser.h
    ImageSource.cpp
    ImageSource.h
    ImageSourceFactories.h
    SetupSensors.h
    FakeImageSource.cpp
    ${OSVR_VIDEOTRACKERSHARED_SOURCES_IO})
if(WIN32)
    list(APPEND PLUGIN_SOURCES
        DirectShowHDKCameraFactory.h
        DirectShowImageSource.cpp
        DirectShowToCV.h)
else()
    list(APPEND PLUGIN_SOURCES
        UVCImageSource.cpp)
endif()

osvr_add_plugin(NAME com_osvr_VideoBasedHMDTracker
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    ${PLUGIN_SOURCES}
)

target_link_libraries(com_osvr_VideoBasedHMDTracker
    vbtracker-core
    vendored-hidapi
    JsonCpp::JsonCpp
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
else()

target_link_libraries(com_osvr_VideoBasedHMDTracker ${libuvc_LIBRARIES} ${LIBUSB1_LIBRARIES})
    target_include_directories(com_osvr_VideoBasedHMDTracker PRIVATE ${libuvc_INCLUDE_DIRS} ${LIBUSB1_INCLUDE_DIRS})
endif()

set_target_properties(com_osvr_VideoBasedHMDTracker PROPERTIES
    FOLDER "OSVR Plugins/Video-Based Tracker")

###
# Calibration utility
###
if(WIN32)
    set(CALIB_SOURCES
        VideoTrackerCalibrationUtility.cpp
        ConfigurationParser.h
        CVTwoStepProgressBar.h
        ImageSource.cpp
        ImageSource.h
        ImageSourceFactories.h
        SetupSensors.h
        DirectShowHDKCameraFactory.h
        DirectShowImageSource.cpp
        DirectShowToCV.h
        ${OSVR_VIDEOTRACKERSHARED_SOURCES_IO})
    add_executable(VideoTrackerCalibrationUtility ${CALIB_SOURCES})
    target_link_libraries(VideoTrackerCalibrationUtility
        PRIVATE
        vbtracker-core
        JsonCpp::JsonCpp
        osvrServer
        directshow-camera)
    set_target_properties(VideoTrackerCalibrationUtility PROPERTIES
        FOLDER "OSVR Plugins/Video-Based Tracker")

    install(TARGETS VideoTrackerCalibrationUtility
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
endif()


if(BUILD_TESTING)
    add_executable(vbtracker-cam
        TestStandalone.cpp)
    target_link_libraries(vbtracker-cam
        PRIVATE
        vbtracker-core)
    set_target_properties(vbtracker-cam PROPERTIES
        FOLDER "OSVR Plugins/Video-Based Tracker")

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

