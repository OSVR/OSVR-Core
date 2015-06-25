# This generates a header file, from the named json file, containing a string literal
# named com_osvr_VideoBasedHMDTracker_json (not null terminated)
# The file must be added as a source file to some target (as below) to be generated.
osvr_convert_json(com_osvr_VideoBasedHMDTracker_json
	com_osvr_VideoBasedHMDTracker.json
	"${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoBasedHMDTracker_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(CORE_VBTRACKER_FILES
	BeaconBasedPoseEstimator.cpp
	BeaconBasedPoseEstimator.h
	GetCameraMatrix.h
	LedIdentifier.cpp
	LedIdentifier.h
	LED.cpp
	LED.h
	Types.h
	VideoBasedTracker.cpp
	VideoBasedTracker.h)

# This is just a helper function wrapping CMake's add_library command that
# sets up include dirs, libraries, and naming convention (no leading "lib")
# for an OSVR plugin. It also installs the plugin into the right directory.
# Pass as many source files as you need. See osvrAddPlugin.cmake for full docs.
osvr_add_plugin(NAME com_osvr_VideoBasedHMDTracker
    MANUAL_LOAD
    CPP # indicates we'd like to use the C++ wrapper
    NO_INSTALL # while in development, don't install
    SOURCES
    ${CORE_VBTRACKER_FILES}
    com_osvr_VideoBasedHMDTracker.cpp
    Oculus_DK2.cpp
    Oculus_DK2.h
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoBasedHMDTracker_json.h"
)

target_link_libraries(com_osvr_VideoBasedHMDTracker
    ${VIDEOTRACKER_EXTRA_LIBS}
    opencv_core
    vendored-vrpn
    eigen-headers)

target_include_directories(com_osvr_VideoBasedHMDTracker PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/vrpn/submodules/hidapi")

set_target_properties(com_osvr_VideoBasedHMDTracker PROPERTIES
    FOLDER "OSVR Plugins")

if(BUILD_TESTING)
    add_executable(vbtracker-cam
        ${CORE_VBTRACKER_FILES}
        TestStandalone.cpp)
    target_link_libraries(vbtracker-cam
        PRIVATE
        osvrPluginKitCpp
        ${VIDEOTRACKER_EXTRA_LIBS}
        opencv_core
        vendored-vrpn
        eigen-headers)
    #osvr_setup_gtest(vbtracker-cam)
endif()
