osvr_setup_lib_vars(VRPNServer)

set(API
    "${HEADER_LOCATION}/GetVRPNConnection.h"
    "${HEADER_LOCATION}/VRPNDeviceRegistration.h")

set(SOURCE
    GetVRPNConnection.cpp
    VRPNDeviceRegistration.cpp)

osvr_add_library()

set_target_properties(${LIBNAME_FULL} PROPERTIES
    VERSION ${OSVR_VERSION}
    SOVERSION 0
    INTERFACE_osvrcore_MAJOR_VERSION 0)

set_property(TARGET ${LIBNAME_FULL} APPEND PROPERTY
    COMPATIBLE_INTERFACE_STRING osvrcore_MAJOR_VERSION)

target_include_directories(${LIBNAME_FULL}
    PUBLIC
    ${Boost_INCLUDE_DIRS})

target_compile_options(${LIBNAME_FULL}
    PUBLIC
    ${OSVR_CXX11_FLAGS})

target_link_libraries(${LIBNAME_FULL}
    PUBLIC
    osvrConnection
    osvrPluginKitCpp
    osvrUtilCpp
    PRIVATE
    osvrPluginHost
    vendored-vrpn)

