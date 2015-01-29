osvr_setup_lib_vars(PluginKit)

set(API
    "${HEADER_LOCATION}/AnalogInterfaceC.h"
    "${HEADER_LOCATION}/ButtonInterfaceC.h"
    "${HEADER_LOCATION}/CommonC.h"
    "${HEADER_LOCATION}/DeviceInterface.h"
    "${HEADER_LOCATION}/DeviceInterfaceC.h"
    "${HEADER_LOCATION}/PluginKit.h"
    "${HEADER_LOCATION}/PluginKitC.h"
    "${HEADER_LOCATION}/PluginRegistration.h"
    "${HEADER_LOCATION}/PluginRegistrationC.h"
    "${HEADER_LOCATION}/TrackerInterfaceC.h")

set(SOURCE
    AnalogDeviceInterface.cpp
    AnalogDeviceInterface.h
    AnalogInterfaceC.cpp
    ButtonInterfaceC.cpp
    DeviceInitObject.cpp
    DeviceInitObject.h
    DeviceInterfaceC.cpp
    HandleNullContext.h
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
    osvrUtilCpp)

###
# C++ (header-only) interface
###
# Requires some version of Boost, but not C++11.
osvr_add_cpp_interface_library()
# In an installed version, we search and add the dependency in the config script
# since we don't need the same version in the same place.
target_include_directories(${LIBNAME_FULL_CPP}
    INTERFACE
    $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)
