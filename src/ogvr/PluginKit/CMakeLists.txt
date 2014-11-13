ogvr_setup_lib_vars(PluginKit)

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
    AnalogInterfaceC.cpp
    ButtonInterfaceC.cpp
    DeviceInitObject.cpp
    DeviceInitObject.h
    DeviceInterfaceC.cpp
    HandleNullContext.h
    PluginRegistrationC.cpp
    TrackerInterfaceC.cpp)

ogvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OGVR_VERSION}
    SOVERSION 0
    INTERFACE_ogvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING ogvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PRIVATE
    ${Boost_INCLUDE_DIRS})

target_link_libraries(${LIBNAME_FULL}
    INTERFACE
    libfunctionality::functionality-plugininterface
    PRIVATE
    ogvrPluginHost
    ogvrConnection
    ogvrUtilCpp)

###
# C++ (header-only) interface
###
# Requires some version of Boost, but not C++11.
ogvr_add_cpp_interface_library()
target_include_directories(${LIBNAME_FULL_CPP}
    INTERFACE ${Boost_INCLUDE_DIRS})
