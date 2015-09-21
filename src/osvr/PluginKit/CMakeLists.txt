osvr_setup_lib_vars(PluginKit)

set(API
    "${HEADER_LOCATION}/AnalogInterfaceC.h"
    "${HEADER_LOCATION}/ButtonInterfaceC.h"
    "${HEADER_LOCATION}/CommonC.h"
    "${HEADER_LOCATION}/DeviceInterface.h"
    "${HEADER_LOCATION}/DeviceInterfaceC.h"
    "${HEADER_LOCATION}/DirectionInterfaceC.h"
    "${HEADER_LOCATION}/EyeTrackerInterfaceC.h"
    "${HEADER_LOCATION}/ImagingInterface.h"
    "${HEADER_LOCATION}/ImagingInterfaceC.h"
    "${HEADER_LOCATION}/Location2DInterfaceC.h"
    "${HEADER_LOCATION}/LocomotionInterfaceC.h"
    "${HEADER_LOCATION}/PluginKit.h"
    "${HEADER_LOCATION}/PluginKitC.h"
    "${HEADER_LOCATION}/PluginRegistration.h"
    "${HEADER_LOCATION}/PluginRegistrationC.h"
    "${HEADER_LOCATION}/TrackerInterfaceC.h")

set(SOURCE
    AnalogInterfaceC.cpp
    ButtonInterfaceC.cpp
    DeviceInterfaceC.cpp
    DirectionInterfaceC.cpp
    HandleNullContext.h
    EyeTrackerInterfaceC.cpp
    ImagingInterfaceC.cpp
    Location2DInterfaceC.cpp
    LocomotionInterfaceC.cpp
    PluginRegistrationC.cpp
    TrackerInterfaceC.cpp)

osvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OSVR_VERSION}
    SOVERSION 0
    INTERFACE_osvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING osvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PRIVATE
    ${Boost_INCLUDE_DIRS})

target_link_libraries(${LIBNAME_FULL}
    INTERFACE
    libfunctionality::functionality-plugininterface
    PRIVATE
    osvrPluginHost
    osvrConnection
    osvrUtilCpp
    osvrCommon
    vendored-vrpn
    boost_thread)

# Add library alias to allow use of the same osvr_add_plugin script within this
# tree and outside of it.
add_library(osvr::${LIBNAME_FULL} ALIAS ${LIBNAME_FULL})

###
# C++ (header-only) interface
###
# Requires some version of Boost, and util for time, but not C++11.
osvr_add_cpp_interface_library()
target_link_libraries(${LIBNAME_INTERFACE}
    INTERFACE
    osvrUtil)

# In an installed version, we search and add the dependency in the
# osvrConfigInstalledBoost.cmake script since we don't need the same version in
# the same place. Thus, here we only add it to the interface if using a build tree.
target_include_directories(${LIBNAME_INTERFACE}
    INTERFACE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)

# Add library alias to allow use of the same osvr_add_plugin script within this
# tree and outside of it.
add_library(osvr::${LIBNAME_INTERFACE} ALIAS ${LIBNAME_INTERFACE})

###
# Imaging interface library
###
osvr_add_interface_library(Imaging)


# In an installed version, we search and add the dependency in the
# osvrConfigInstalledOpenCV.cmake script since we don't need the same version in
# the same place. Thus, here we only add it to the interface if using a build tree.
target_link_libraries(${LIBNAME_INTERFACE}
    INTERFACE
    ${LIBNAME_FULL}Cpp
    $<BUILD_INTERFACE:opencv_core>)

# Add library alias to allow use of the same osvr_add_plugin script within this
# tree and outside of it.
add_library(osvr::${LIBNAME_INTERFACE} ALIAS ${LIBNAME_INTERFACE})
