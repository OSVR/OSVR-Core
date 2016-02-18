set(VIDEOTRACKER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../videobasedtracker")
set(PROJ_FOLDER "OSVR Plugins/Unified Video-Inertial Tracker")
osvr_convert_json(org_osvr_unifiedvideoinertial_json
    org_osvr_unifiedvideoinertial.json
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_unifiedvideoinertial_json.h")

# Be able to find our generated header file, have shared files find us, and have us find shared files.
include_directories("${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${OSVR_VIDEOTRACKERSHARED_INCLUDE_DIR}")

# Make sure we can link these upcoming static libraries into a shared lib
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(ImageSources)

###
# Static library with core of the tracker
###
add_library(uvbi-core STATIC
    ApplyIMUToState.cpp
    ApplyIMUToState.h
    Assumptions.h
    BodyIdTypes.h
    BeaconIdTypes.h
    BeaconSetupData.cpp
    BeaconSetupData.h
    BodyTargetInterface.h
    CannedIMUMeasurement.h
    ConfigParams.cpp
    ConfigParams.h
    ForEachTracked.h
    HDKLedIdentifier.cpp
    HDKLedIdentifier.h
    HDKLedIdentifierFactory.cpp
    HDKLedIdentifierFactory.h
    HistoryContainer.h
    ImageProcessing.h
    ImagePointMeasurement.h
    LED.cpp
    LED.h
    LedIdentifier.cpp
    LedIdentifier.h
    ModelTypes.h
    PoseEstimator_RANSAC.cpp
    PoseEstimator_RANSAC.h
    PoseEstimator_RANSACKalman.cpp
    PoseEstimator_RANSACKalman.h
    PoseEstimator_SCAATKalman.cpp
    PoseEstimator_SCAATKalman.h
    PoseEstimatorTypes.h
    RoomCalibration.cpp
    RoomCalibration.h
    SpaceTransformations.h
    StateHistory.h
    TimeValueChrono.h
    TrackedBody.cpp
    TrackedBody.h
    TrackedBodyIMU.cpp
    TrackedBodyIMU.h
    TrackedBodyTarget.cpp
    TrackedBodyTarget.h
    TrackingDebugDisplay.cpp
    TrackingDebugDisplay.h
    TrackingSystem_Impl.cpp
    TrackingSystem_Impl.h
    TrackingSystem.cpp
    TrackingSystem.h
    Types.h
    ${OSVR_VIDEOTRACKERSHARED_SOURCES_CORE})
target_compile_options(uvbi-core
    PUBLIC
    ${OSVR_CXX11_FLAGS})
target_link_libraries(uvbi-core
    PUBLIC
    ${VIDEOTRACKER_EXTRA_LIBS}
    opencv_core
    osvrUtilCpp # for typedefs and boost headers
    osvrKalman
    eigen-headers
    PRIVATE
    util-headers)
set_target_properties(uvbi-core PROPERTIES
    FOLDER "${PROJ_FOLDER}")

###
# Convenience tool to view the tracker camera using the same pipeline as the tracker plugin.
###
add_executable(uvbi-view-camera ViewTrackingCamera.cpp)
target_link_libraries(uvbi-view-camera PRIVATE uvbi-image-sources)
set_target_properties(uvbi-view-camera PROPERTIES
    FOLDER "${PROJ_FOLDER}")

osvr_add_plugin(NAME org_osvr_unifiedvideoinertial
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    org_osvr_unifiedvideoinertial.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_unifiedvideoinertial_json.h"
    ConfigurationParser.h
    MakeHDKTrackingSystem.h
    ThreadsafeBodyReporting.cpp
    ThreadsafeBodyReporting.h
    TrackerThread.cpp
    TrackerThread.h
    ${OSVR_VIDEOTRACKERSHARED_SOURCES_HDKDATA}
    ${OSVR_VIDEOTRACKERSHARED_SOURCES_IO})

target_link_libraries(org_osvr_unifiedvideoinertial
    osvr::osvrAnalysisPluginKit
    uvbi-core
    uvbi-image-sources
    JsonCpp::JsonCpp
    util-headers)

set_target_properties(org_osvr_unifiedvideoinertial PROPERTIES
    FOLDER "${PROJ_FOLDER}")
