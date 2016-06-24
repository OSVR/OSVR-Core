###
# Build a static library to read from an DirectShow camera.
# @todo This will be removed when OpenCV knows how to read from the
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
    base_camera_server.h
    base_camera_server.cpp
    ConnectTwoFilters.h
    directx_camera_server.h
    directx_camera_server.cpp
    directx_samplegrabber_callback.h
    directx_samplegrabber_callback.cpp
    GetIVideoProcAmp.h
    GetPin.h
    MediaSampleExchange.h
    MediaSampleExchange.cpp
    NullRenderFilter.cpp
    NullRenderFilter.h
    PropertyBagHelper.h
    SampleGrabberWrapper.h
    SampleGrabberWrapper.cpp
    SignalEvent.h)
target_include_directories(directshow-camera
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(directshow-camera
    PUBLIC
    vendored-vrpn
    comutils
    osvrUtil
    PRIVATE
    osvrUtilCpp
    osvr_codecvt
    ${DIRECTSHOW_STRMIIDS_LIBRARY}
    )

target_compile_options(directshow-camera
    PUBLIC
    ${OSVR_CXX11_FLAGS})
set_target_properties(directshow-camera PROPERTIES
    FOLDER "OSVR Plugins/Video-Based Tracker")
