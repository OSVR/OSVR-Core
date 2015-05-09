# This generates a header file, from the named json file, containing a string literal
# named com_osvr_VideoBasedHMDTracker_json (not null terminated)
# The file must be added as a source file to some target (as below) to be generated.
osvr_convert_json(com_osvr_VideoBasedHMDTracker_json
    com_osvr_VideoBasedHMDTracker.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoBasedHMDTracker_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# This is just a helper function wrapping CMake's add_library command that
# sets up include dirs, libraries, and naming convention (no leading "lib")
# for an OSVR plugin. It also installs the plugin into the right directory.
# Pass as many source files as you need. See osvrAddPlugin.cmake for full docs.
osvr_add_plugin(NAME com_osvr_VideoBasedHMDTracker
    MANUAL_LOAD
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    com_osvr_VideoBasedHMDTracker.cpp
    Oculus_DK2.cpp
	Oculus_DK2.h
	LedIdentifier.cpp
	LedIdentifier.h
	LED.cpp
	LED.h
	BeaconBasedPoseEstimator.cpp
	BeaconBasedPoseEstimator.h
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoBasedHMDTracker_json.h"
)

target_link_libraries(com_osvr_VideoBasedHMDTracker opencv_features2d opencv_core opencv_highgui opencv_imgproc vendored-vrpn)

target_include_directories(com_osvr_VideoBasedHMDTracker PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/vrpn/submodules/hidapi")

set_target_properties(com_osvr_VideoBasedHMDTracker PROPERTIES
    FOLDER "OSVR Plugins")

