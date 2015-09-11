# This generates a header file, from the named json file, containing a string literal
# named com_osvr_VideoBasedHMDTracker_json (not null terminated)
# The file must be added as a source file to some target (as below) to be generated.
osvr_convert_json(com_osvr_VideoBasedHMDTracker_json
    com_osvr_VideoBasedHMDTracker.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoBasedHMDTracker_json.h")

# Be able to find our generated header file.
include_directories("${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/DirectShowCameraLib")

###
# Build a separate static library with the core of the video-based tracker
###
add_library(vbtracker-core STATIC
    BeaconBasedPoseEstimator.cpp
    BeaconBasedPoseEstimator.h
    GetCameraMatrix.h
    HDKLedIdentifier.cpp
    HDKLedIdentifier.h
    HDKLedIdentifierFactory.cpp
    HDKLedIdentifierFactory.h
    IdentifierHelpers.h
    LedIdentifier.cpp
    LedIdentifier.h
    LED.cpp
    LED.h
    Types.h
    VideoBasedTracker.cpp
    VideoBasedTracker.h)

target_link_libraries(vbtracker-core
    PUBLIC
    ${VIDEOTRACKER_EXTRA_LIBS}
    opencv_core
    osvrUtilCpp # for typedefs and boost headers
    vendored-vrpn # for quatlib
    eigen-headers)
target_compile_options(vbtracker-core
    PUBLIC
    ${OSVR_CXX11_FLAGS})

set_target_properties(vbtracker-core PROPERTIES
    FOLDER "OSVR Plugins/Video-Based Tracker")


if(WIN32)
    ###
    # Build a static library to read from an DirectShow camera.
    # @toto This will be removed when OpenCV knows how to read from the
    # HDK camera.
    #
    # qedit.h
    # MSVC: Needs Microsoft Windows SDK Update for Windows Vista and .NET Framework 3.0 or older
    # See https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/2ab5c212-5824-419d-b5d9-7f5db82f57cd
    # (though the DL link there is wrong)
    # They also have a workaround header posted there, but sadly with no license/copyright info.
    # (Hasn't stopped it from spreading into other projects online, though)
    ###

    find_package(DirectShow QUIET)
    if(NOT DIRECTSHOW_STRMIIDS_LIBRARY)
        set(DIRECTSHOW_STRMIIDS_LIBRARY strmiids)
    endif()
    if(DIRECTSHOW_QEDIT_INCLUDE_DIR)
        set(QEDIT_WRAPPER_INCLUDE_FILE "${DIRECTSHOW_QEDIT_INCLUDE_DIR}/qedit.h")
    endif()
    configure_file(qedit_wrapper.h.cmake_in "${CMAKE_CURRENT_BINARY_DIR}/qedit_wrapper.h" @ONLY)

    add_library(directshow-camera STATIC
        DirectShowCameraLib/base_camera_server.h
        DirectShowCameraLib/base_camera_server.cpp
        DirectShowCameraLib/directx_camera_server.h
        DirectShowCameraLib/directx_camera_server.cpp
        DirectShowCameraLib/WinVariant.h
        )
    target_include_directories(directshow-camera PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
    target_link_libraries(directshow-camera
        PUBLIC
        vendored-vrpn
        com_smart_pointer
        PRIVATE
        osvrUtilCpp
        ${OSVR_CODECVT_LIBRARIES}
        ${DIRECTSHOW_STRMIIDS_LIBRARY}
        )

    target_compile_options(directshow-camera
        PUBLIC
        ${OSVR_CXX11_FLAGS})
    set_target_properties(directshow-camera PROPERTIES
        FOLDER "OSVR Plugins/Video-Based Tracker")
endif()
###
# Build the plugin
###
osvr_add_plugin(NAME com_osvr_VideoBasedHMDTracker
    MANUAL_LOAD
    CPP # indicates we'd like to use the C++ wrapper
    SOURCES
    com_osvr_VideoBasedHMDTracker.cpp
    Oculus_DK2.cpp
    Oculus_DK2.h
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_VideoBasedHMDTracker_json.h"
)

target_link_libraries(com_osvr_VideoBasedHMDTracker
    vbtracker-core
    vendored-hidapi
)
if(WIN32)
    target_link_libraries(com_osvr_VideoBasedHMDTracker
        directshow-camera
    )
endif()

set_target_properties(com_osvr_VideoBasedHMDTracker PROPERTIES
    FOLDER "OSVR Plugins/Video-Based Tracker")

if(BUILD_TESTING)
    add_executable(vbtracker-cam
        TestStandalone.cpp)
    target_link_libraries(vbtracker-cam
        PRIVATE
        vbtracker-core
        directshow-camera)
    set_target_properties(vbtracker-cam PROPERTIES
        FOLDER "OSVR Plugins/Video-Based Tracker")
    #osvr_setup_gtest(vbtracker-cam)
endif()
